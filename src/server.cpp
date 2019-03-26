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

    char buffer[256];
    bzero(&buffer, 255);

    ret = read(conn_fd, buffer, 255);
    if(ret < 0) {
        logger.error("Could not read from the new connection");
        throw SocketReadException();
    }

    std::string msg = std::string(buffer);
    logger.info("New message received: " + msg);

    std::string response = handleMessage(msg);
    ret = write(conn_fd, response.c_str(), response.length());

    if(ret < 0) {
        logger.error("Could not write to the connection");
        throw SocketWriteException();
    }
    close(conn_fd);
    conn_mtx.unlock();
}

std::string ElevServer::handleMessage(const std::string &msg) {
    command_t cmd;
    elev_data_t data;
    logger.info("Tokenizing message");
    std::vector<std::string> t = tokenizeMessage(msg);
    logger.info("Parsing message");
    cmd = parseMessage(t);
    logger.info("Executing commands");
    data = executeCommand(cmd);
    logger.info("Crafting response");
    return createResponse(cmd, data);
}

std::string ElevServer::createResponse(command_t cmd, elev_data_t data) {
    std::string response = "";
    if(data.valid) {
        response = std::to_string(data.data);
    } else {
        response =  "ERROR - Command not recognized!\n"
                    "usage:\n"
                    "   get/set [COMMAND]\n"
                    "commands:\n";
        for(auto it = commands.begin(); it != commands.end(); ++it) {
            response += "\t";
            response += it->first;
            response += "\n";
        }
            /*
                "   BUTTON_COMMAND_1,"
                "   BUTTON_COMMAND_2,"
                "   BUTTON_COMMAND_3,"
                "   BUTTON_COMMAND_4,"
                "   BUTTON_UP_1,"
                "   BUTTON_UP_2,"
                "   BUTTON_UP_3, "
                "   BUTTON_DOWN_2,"
                "   BUTTON_DOWN_3,"
                "   BUTTON_DOWN_4,"
                "   STOP_BUTTON,"
                "   OBSTRUCTION,"
                "   FLOOR_SENSOR,"
                "   DOOR_LAMP,"
                "   LIGHT_STOP,"
                "   LIGHT_COMMAND_1,"
                "   LIGHT_COMMAND_2,"
                "   LIGHT_COMMAND_3,"
                "   LIGHT_COMMAND_4,"
                "   LIGHT_UP_1,"
                "   LIGHT_UP_2,"
                "   LIGHT_UP_3,"
                "   LIGHT_DOWN_2,"
                "   LIGHT_DOWN_3,"
                "   LIGHT_DOWN_4,"
                "   LIGHT_FLOOR_1,"
                "   LIGHT_FLOOR_2,"
                "   LIGHT_FLOOR_3,"
                "   LIGHT_FLOOR_4,"
                "   MOTOR_DIR,"
                "   MOTOR_SPEED";
                */
    }
    return response;
}
std::vector<std::string> ElevServer::tokenizeMessage(std::string msg) {
    //msg.erase(msg.end()-1); //This only seems to be nessecary if the socket sends garbage data at the end.
    std::vector<std::string> tokens;
    std::stringstream ss(msg);
    std::string token;
    
    while(std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
    return tokens;
}

command_t ElevServer::parseMessage(const std::vector<std::string> &tokens) {
    /*
    Message format:
        elevator_id <value, default 0> get/set command <which_instance> <data> 
    */
    //TODO: This needs some cleanup and/or a neater solution
    command_t cmd{-1, CommandType::GET, Signals::NUM_SIGNALS, 0};
    for(auto it = tokens.begin(); it != tokens.end(); ++it) {
        logger.info("Parsing token: [" + *it + "]");
        if(*it == "id") {
            try {
                cmd.id = std::stoi(*(++it));
            } catch(const std::exception &e) {
                logger.error("Malformed message received");
            }
        } else if(*it == "get") {
            cmd.cmd_type = CommandType::GET;
        } else if(*it == "set") {
            cmd.cmd_type = CommandType::SET;
        } else {
            try {
                cmd.signal = commands.at(*it);
                if(cmd.cmd_type == CommandType::SET) {
                    try {
                        if((it+1) == tokens.end()) {
                            cmd.value = 0;
                        } else {
                            cmd.value = std::stoi(*(++it));
                        }
                    } catch(const std::invalid_argument &e) {
                        logger.error("Could not convert argument to int");
                        cmd.value = 0;
                    }
                }
            } catch (const std::out_of_range &e){
               logger.error("Misformed command: [" + *it + "]");
            }

        }
    }
    if(cmd.id < 0) {
        cmd.id = 0;
    }
    return cmd;
}

elev_data_t ElevServer::executeCommand(const command_t &cmd) {
    std::shared_ptr<elev::Elevator> e_ptr;
    elev_data_t d{0, true};

    if (cmd.signal == Signals::NUM_SIGNALS) {
        d.valid = false;
        return d;
    }

    for(auto e: elevators) { 
        if(e->getId() == cmd.id) {
            e_ptr = e;
        }
    }

    if(cmd.cmd_type == CommandType::SET) {
       d.data = e_ptr->setSignal(cmd.signal, cmd.value);

    } else if(cmd.cmd_type == CommandType::GET) {
        d.data = e_ptr->getSignal(cmd.signal);

    } else {
        logger.error("Malformed command received");
        d.valid = false;

    }

    return d;
}

bool ElevServer::ok(void) {
    std::lock_guard<std::mutex> lock(ok_mtx);
    return running;
}

void ElevServer::stop(void) {
    std::lock_guard<std::mutex> lock(ok_mtx);
    running = false;
}
