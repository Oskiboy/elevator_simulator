#include <string>
#include <sstream>
#include <memory>
#include <iostream>
#include <thread>
#include <csignal>

#include <server.hpp>
#include <socket_exception.hpp>

#include <string.h>
#include <unistd.h>

ElevServer::ElevServer(int port, int num_of_elevators, std::string log_file):
logger(log_file), running(false), port_num(port)  
{
    ElevServer::stop();
    //Creating a new TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        logger.error("Could not create socket");
        throw SocketCreateException();
    }
    logger.info("Socket created");

    for(int i = 0; i < num_of_elevators; ++i) {
        elevators.push_back(std::make_shared<elev::Elevator>(i)); 
    }

    runThreads();
    
    logger.info("Elevators set up");

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
}

ElevServer::~ElevServer() {
    close(server_fd);
    close(conn_fd);
}

void ElevServer::run(void) {
    running = true;
    int ret;
    
    ret = bind(server_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if(ret < 0) {
        logger.error("Could not bind the socket");
        throw SocketBindException();
    }
    logger.info("Socket bound to port " + std::to_string(port_num));
    
    listen(server_fd, 5);
    logger.info("Socket now listening");
    
    
    while(ok()) {
        handleConnection();
    }

    for(auto e: elevators) {
        e->stop();
    }
    logger.info("All elevators stopped");

    joinThreads();
    logger.info("All threads joined.");
    close(server_fd);
    close(conn_fd);
    logger.info("All sockets closed");
}

void ElevServer::runThreads() {
    std::string msg;
    for(auto e: elevators) {
        msg = "Running thread for elevator id: ";
        msg += std::to_string(e->getId());
        logger.info(msg);

        thread_pool.push_back(std::thread(&elev::Elevator::run, e.get()));
        logger.info("New thread started");
    }
}

void ElevServer::joinThreads() {
    for(auto it = thread_pool.begin(); it != thread_pool.end(); ++it) {
        if(it->joinable()) {
            it->join();
        }
    }
}


void ElevServer::handleConnection() {
    conn_mtx.lock();
    int ret;
    cli_len = sizeof(cli_addr);

    fd_set fdread;
    struct timeval tv;

    tv.tv_sec=0;
    tv.tv_usec=500000;

    FD_ZERO(&fdread);
    FD_SET(server_fd, &fdread);
    int select_status = select(server_fd + 1, &fdread, 0, 0, &tv);

    if(select_status < 0) {
        logger.error("Select error");
        throw SocketAcceptException();
    } else if(select_status == 0) {
        conn_mtx.unlock();
        return;
    }

    conn_fd = accept(server_fd, (struct sockaddr*) &cli_addr, &cli_len);
    logger.info("New connection!");

    if(conn_fd < 0) {
        logger.error("Could not accept connection");
        throw SocketAcceptException();
    }

    char buffer[4];
    bzero(&buffer, 4);

    ret = read(conn_fd, buffer, 4 * sizeof(buffer[0]));
    if(ret < 0) {
        logger.error("Could not read from the new connection");
        throw SocketReadException();
    }

    std::string msg = std::string(buffer);
    logger.info("New message received: " + msg);

    std::string response = handleMessage(buffer);
    if(buffer[0] > 5) {
        ret = write(conn_fd, response.c_str(), response.length());
    }

    if(ret < 0) {
        logger.error("Could not write to the connection");
        throw SocketWriteException();
    }
    close(conn_fd);
    conn_mtx.unlock();
}

std::string ElevServer::handleMessage(const char msg[4]) {
    command_t cmd;
    logger.info("Parsing message");
    cmd = parseMessage(msg);
    logger.info("Executing commands");
    cmd = executeCommand(cmd);
    logger.info("Crafting response");
    return createResponse(cmd);
}

std::string ElevServer::createResponse(command_t cmd) {
    char response[4] = {0, 0, 0, 0};
    if(cmd.cmd != CommandType::ERROR) {
        response[0] = cmd.msg[0];
        response[1] = cmd.value;
        response[2] = cmd.floor;
        response[3] = cmd.msg[3];
    } else {
        response[0] = -1;
    }
    return std::string(response);
}


command_t ElevServer::parseMessage(const char msg[4]) {
    /*
    Message format:
        elevator_id <value, default 0> get/set command <which_instance> <data> 
    */
    //TODO: This needs some cleanup and/or a neater solution
    command_t cmd{CommandType::ERROR, CommandSignal::NUM_SIGNALS, 0,0,0,0,0};
    switch (msg[0])
    {
        case 1:
            cmd.signal = CommandSignal::MOTOR;
            break;
        case 2:
            cmd.signal = CommandSignal::BUTTON;
            cmd.selector = msg[1];
            break;
        case 3:
            cmd.signal = CommandSignal::FLOOR_SENSOR;
            break;
        case 4:
            cmd.signal = CommandSignal::DOOR_LIGHT;
            break;
        case 5:
            cmd.signal = CommandSignal::STOP_LIGHT;
            break;
        case 6:
            cmd.signal = CommandSignal::BUTTON;
            cmd.selector = msg[1];
            break;
        case 7:
            cmd.signal = CommandSignal::FLOOR_SENSOR;
            break;
        case 8:
            cmd.signal = CommandSignal::STOP;
            break;
        case 9:
            cmd.signal = CommandSignal::OBSTRUCTION;
            break;
        default:
            cmd.signal = CommandSignal::NUM_SIGNALS;
            break;
    }
    cmd.cmd =       ((msg[0] > 5) ? CommandType::GET : CommandType::SET);
    cmd.floor =     msg[2];
    cmd.value =     msg[3];
    cmd.selector =  msg[2];
    cmd.elevator_num = 0;

    return cmd;
}

command_t ElevServer::executeCommand(const command_t &cmd) {
    std::shared_ptr<elev::Elevator> e_ptr;
    command_t ret{};
    for(auto e: elevators) { 
        if(e->getId() == cmd.elevator_num) {
            e_ptr = e;
        }
    }

    if(cmd.cmd == CommandType::SET) {
        ret = e_ptr->executeCommand(cmd);

    } else if(cmd.cmd == CommandType::GET) {
        ret = e_ptr->executeCommand(cmd);

    } else {
        logger.error("Malformed command received");
        ret.cmd = CommandType::ERROR;
    }

    return ret;
}

bool ElevServer::ok(void) {
    std::lock_guard<std::mutex> lock(ok_mtx);
    return running;
}

void ElevServer::stop(void) {
    std::lock_guard<std::mutex> lock(ok_mtx);
    running = false;
}