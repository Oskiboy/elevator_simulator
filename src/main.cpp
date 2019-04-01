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
    std::cout << "Starting server..." << std::endl;
    try {
        e_ptr = std::make_shared<ElevServer>(6060, 1, "logs/test.log");
        server = std::thread(&ElevServer::run, e_ptr);
        std::cout << "Server started!" << std::endl;
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }

    while(running)
        ;
    
    std::cout << "Sending stop signal to server..." << std::endl;
    e_ptr->stop();

    std::cout << "Waiting for threads to join..." << std::endl;
    if(server.joinable()) {
        server.join();
    }
    std::cout << "Server shut down!" << std::endl;
    return 0;
}
