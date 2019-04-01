#include <server_commander.hpp>

ServerCommander::ServerCommander(int num_elevators, int num_floors):
    running(false), num_floors(num_floors), num_elevators(num_elevators)
{
    server = std::make_shared<ElevServer>(6060, num_elevators, "logs/commander.log");
    thd = std::thread(&ElevServer::run, server);
}

void ServerCommander::run() {
    running = true;
    while(running) {
        std::cout << ">>> ";
        std::getline(std::cin, cmd, '\n');
        tokenizeInput();
        executeCommands();
    }
}

void ServerCommander::stop() {
    running = false;
}

void ServerCommander::tokenizeInput() {
    std::string token;
    tokens.clear();

    std::stringstream ss(cmd);
    while(std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
}

void ServerCommander::executeCommands() {
    if(tokens[0] == "pos") {
        std::cout << server->getPosition(0) << std::endl;
    } else if(tokens[0] == "cmd") {
        try{
            for(int i = 0; i < 4; ++i) {
                msg[i] = std::stoi(tokens[i+1]);
            }
        } catch(const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        server->elevControl(msg);
    } else if(tokens[0] == "q" || tokens[0] == "quit") {
        std::cout << "Sending stop signal to the server" << std::endl;
        server->stop();
        if(thd.joinable()) {
            thd.join();
        }
        running = false;
        std::cout << "Exiting" << std::endl;
    }
}