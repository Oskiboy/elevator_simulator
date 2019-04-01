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
    initSocket();

    for(int i = 0; i < num_of_elevators; ++i) {
        elevators.push_back(std::make_shared<elev::Elevator>(i)); 
    }
    logger.info("Elevators set up");
    runThreads();
    logger.info("Elevators running!");
}
/*
ElevServer::~ElevServer() {
    disconnectSocket();
}
*/
void ElevServer::run(void) {
    ok_mtx.lock();
    running = true;
    ok_mtx.unlock();

    connectSocket();
    
    while(ok()) {
        handleConnections();
    }

    stopElevators();
    logger.info("All elevators stopped");

    joinThreads();
    logger.info("All threads joined.");

    disconnectSocket();
    logger.info("All sockets closed");
}

void ElevServer::stopElevators() {
    for(auto e: elevators) {
        e->stop();
    }
}


bool ElevServer::ok(void) {
    std::lock_guard<std::mutex> lock(ok_mtx);
    return running;
}

void ElevServer::stop(void) {
    std::lock_guard<std::mutex> lock(ok_mtx);
    running = false;
}

void ElevServer::elevControl(char msg[4]) {
    command_t cmd = parseMessage(msg);
    command_t ret = executeCommand(cmd);
    std::cout << ret << std::endl;
}

void ElevServer::runThreads() {
    std::string msg;
    for(auto e: elevators) {
        msg = "Running thread for elevator id: ";
        msg += std::to_string(e->getId());
        logger.info(msg);

        thread_pool.emplace_back(&elev::Elevator::run, e.get());
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

void ElevServer::initSocket() {
    //Creating a new TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        logger.error("Could not create socket");
        throw SocketCreateException();
    }
    logger.info("Socket created");
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

}

void ElevServer::connectSocket() {
    int ret = 0;  
    ret = bind(server_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if(ret < 0) {
        logger.error("Could not bind the socket");
        throw SocketBindException();
    }
    logger.info("Socket bound to port " + std::to_string(port_num));
    listen(server_fd, 5);
    logger.info("Socket now listening");
}

void ElevServer::disconnectSocket() {
    int ret = close(server_fd);
    if(ret < 0) {
        logger.error("Could not close server socket! ERROR CODE: " + std::to_string(ret));
    }
    ret = close(conn_fd);
    if(ret < 0) {
        logger.error("Could not close client socket! ERROR CODE: " + std::to_string(ret));
    }
}

int ElevServer::selectSocket() {
    //Set up a timeout and check for active socket connections.
    fd_set fdread;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec= 500000;

    FD_ZERO(&fdread);
    FD_SET(server_fd, &fdread);
    return select(server_fd + 1, &fdread, 0, 0, &tv);
}

void ElevServer::handleConnections() {
    conn_mtx.lock();
    int ret;
    
    int select_status = selectSocket();

    if(select_status < 0) {
        logger.error("Select error");
        throw SocketAcceptException();
    } else if(select_status == 0) {
        conn_mtx.unlock();
        return;
    }

    //If there is an active connection, accept it.
    cli_len = sizeof(cli_addr);
    conn_fd = accept(server_fd, (struct sockaddr*) &cli_addr, &cli_len);
    if(conn_fd < 0) {
        logger.error("Could not accept connection");
        throw SocketAcceptException();
    }

    //Initialize and zero out a buffer for reading from the socket
    char buffer[4];
    bzero(&buffer, 4);
    //Read the 4 bytes from the socket
    ret = read(conn_fd, buffer, 4 * sizeof(buffer[0]));
    if(ret < 0) {
        logger.error("Could not read from the new connection");
        throw SocketReadException();
    }

    std::string msg = "[";
    for(int i = 0; i < 4; ++i) {
        msg += std::to_string(buffer[i]) + " ";
    }
    logger.info("New message received: " + msg + "]");

    //Handle the received message.
    char *response = handleMessage(buffer);
    
    //If the command type is more than five the connection expects an answer.
    if(buffer[0] > 5 && buffer[0] != -1) {
        ret = write(conn_fd, response, 4*sizeof(char));
        if(ret < 0) {
            logger.error("Could not write to the connection");
            throw SocketWriteException();
        }
    } else if((unsigned char)buffer[0] == 255) {
        ret = write(conn_fd, buffer, 4*sizeof(char));
        if(ret < 0) {
            logger.error("Could not write to the connection");
            throw SocketWriteException();
        }
        ret = write(conn_fd, (double*)&response[4], sizeof(double));
        if(ret < 0) {
            logger.error("Could not write to the connection");
            throw SocketWriteException();
        }
    }

    close(conn_fd);
    conn_mtx.unlock();
}

char* ElevServer::handleMessage(const char msg[4]) {
    command_t cmd, ret;
    cmd = parseMessage(msg);
    ret = executeCommand(cmd);
    return createResponse(ret);
}

char* ElevServer::createResponse(const command_t &cmd) {
    static char response[4*sizeof(char) + sizeof(double)];
    bzero(&response, sizeof(response));

    if(cmd.cmd != CommandType::ERROR) {
        response[0] = cmd.msg[0];
        response[1] = cmd.value;
        response[2] = cmd.floor;
        response[3] = cmd.msg[3];
    } else {
        response[0] = 255;
        response[1] = 255;
        response[2] = 255;
        response[3] = 255;
    }

    if((unsigned char)cmd.msg[0] == 255) {
        memcpy(&response[4], &cmd.position, sizeof(cmd.position));
    }

    return response;
}

command_t ElevServer::parseMessage(const char msg[4]) {
    //TODO: This needs some cleanup and/or a neater solution

    //Zero initialize a new command
    command_t cmd { 
        .cmd        = (msg[0] > 5) ? CommandType::GET : CommandType::SET,
        .signal     = CommandSignal::NUM_SIGNALS,
        .selector   = msg[2],
        .floor      = msg[2],
        .value      = msg[3],
        .elevator_num = 0,
        .position   = 0,
        .msg = {
            msg[0], 
            msg[1], 
            msg[2], 
            msg[3]
        }
    };

    switch ((unsigned char)msg[0])
    {
        case 1:
            cmd.signal  = CommandSignal::MOTOR;
            cmd.value   = msg[1];
            break;
        case 2:
            cmd.signal      = CommandSignal::BUTTON;
            cmd.selector    = msg[1];
            cmd.floor       = msg[2];
            cmd.value       = msg[3];
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
            cmd.signal      = CommandSignal::BUTTON;
            cmd.selector    = msg[1];
            cmd.floor       = msg[2];
            cmd.value       = msg[3];
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
        case 10:
            cmd.signal  = CommandSignal::BUTTON;
            cmd.cmd     = CommandType::SET;
            break;
        case 255:
            cmd.signal  = CommandSignal::POSITION;
            cmd.cmd     = CommandType::GET;
            break;
        default:
            cmd.signal = CommandSignal::NUM_SIGNALS;
            break;
    }

    return cmd;
}

command_t ElevServer::executeCommand(const command_t &cmd) {
    std::shared_ptr<elev::Elevator> e_ptr;
    command_t ret {  
        .cmd            = CommandType::ERROR, 
        .signal         = CommandSignal::NUM_SIGNALS,
        .selector       = 0,
        .floor          = 0,
        .value          = 0,
        .elevator_num   = 0,
        .position       = 0,
        .msg = {
            cmd.msg[0], 
            cmd.msg[1], 
            cmd.msg[2], 
            cmd.msg[3]
        }
    };

    //Select the appropriate elevator.
    for(auto e: elevators) { 
        if(e->getId() == cmd.elevator_num) {
            e_ptr = e;
        }
    }

    //If the command type is valid, run command on the elevator.
    if(cmd.cmd == CommandType::ERROR) {
        logger.error("Malformed command received");
        ret.cmd = CommandType::ERROR;
    } else {
        ret = e_ptr->executeCommand(cmd);
    }

    return ret;
}

double ElevServer::getPosition(int id) {
    for(auto e: elevators) {
        if(e->getId() == id)
            return e->getElevatorPosition(); 
    }
    return 0;
}