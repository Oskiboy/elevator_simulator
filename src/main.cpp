#include <iostream>
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
        e_ptr = std::make_shared<ElevServer>(6060, 1, "test.log");
        server = std::thread(&ElevServer::run, e_ptr);
        std::cout << "Server started!" << std::endl;
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    while(running && cmd != "quit" && cmd != "q") {
        std::cout << ">>> ";
        std::cin >> cmd;
    }
    
    std::cout << "Sending stop signal to server..." << std::endl;
    e_ptr->stop();

    std::cout << "Waiting for threads to join" << std::endl;
    if(server.joinable()) {
        server.join();
    }

    return 0;
}
