#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <signal.h>

#include "elevator.hpp"
#include "server.hpp"

std::atomic_bool running(true);

void sigHandler(int num) {
    running = false;
    std::cout << "Stopping server!" << std::endl;
}

int main(int argc, char** argv) {
    signal(SIGINT, sigHandler);
    signal(SIGABRT, sigHandler);

    std::shared_ptr<ElevServer> e_ptr;
    std::thread server;
    std::string cmd = "";
    std::cout << "Starting server..." << std::endl;
    try {
        e_ptr = std::make_shared<ElevServer>(6060, 1, "logs/test.log");
        server = std::thread(&ElevServer::run, e_ptr);
        std::cout << "Server started!" << std::endl;
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }


    std::vector<std::string> tokens;
    std::string token;
    while(running) {
        tokens.clear();
        std::cout << ">>> ";
        std::getline(std::cin, cmd, '\n');
        
        std::stringstream ss(cmd);
        while(std::getline(ss, token, ' ')) {
            tokens.push_back(token);
        }

        if(tokens[0] == "pos") {
            std::cout << e_ptr->getPosition(0) << std::endl;
        } else if(tokens[0] == "cmd") {
            char msg[4] = {0, 0, 0, 0};
            try{
                for(int i = 0; i < 4; ++i) {
                    msg[i] = std::stoi(tokens[i+1]);
                }
            } catch(const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
             
            e_ptr->elevControl(msg);
        } else if(tokens[0] == "q" || tokens[0] == "quit") {
            break;
        }
    }
    
    std::cout << "Sending stop signal to server..." << std::endl;
    e_ptr->stop();

    std::cout << "Waiting for threads to join" << std::endl;
    if(server.joinable()) {
        server.join();
    }

    return 0;
}
