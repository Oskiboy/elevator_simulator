#ifndef SERVER_COMMANDER_HPP
#define SERVER_COMMANDER_HPP
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include <atomic>

#include <server.hpp>

class ServerCommander {
    public:
        ServerCommander(int num_elevators, int num_floors);
        ServerCommander() = delete;
        ServerCommander& operator=(const ServerCommander&) = delete;
        ServerCommander(const ServerCommander&) = delete;
        void run();
        void stop();
    private:
        void tokenizeInput();
        void executeCommands();

    private:
        std::atomic_bool running;

        int num_floors, num_elevators;

        std::vector<std::string> tokens;
        std::string cmd;
        char msg[4];

        std::shared_ptr<ElevServer> server;
        std::thread thd;
};

#endif //SERVER_COMMANDER_HPP
