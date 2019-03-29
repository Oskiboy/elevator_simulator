#ifndef ELEV_SERVER_HPP
#define ELEV_SERVER_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <map>

#include <elevator.hpp>
#include <logger.hpp>
#include <signals.hpp>
#include <commands.hpp>

#include <sys/socket.h>
#include <netinet/in.h>

namespace elev {
class Elevator;
}

class ElevServer {
    public:
        ElevServer(int port, int num_of_elevators, std::string log_file);
        ~ElevServer();

        void    run(void);
        bool    ok(void);
        void    stop(void);
        
    private:
        void        handleConnection();
        std::string handleMessage(const char msg[4]);
        command_t   parseMessage(const char msg[4]);
        command_t   executeCommand(const command_t &cmd);
        std::string createResponse(command_t cmd);
        
        void runThreads();
        void joinThreads();

    private:
        Logger logger;
        bool running;
        
        std::mutex conn_mtx, ok_mtx;
        std::vector< std::shared_ptr<elev::Elevator> > elevators;
        std::vector<std::thread> thread_pool;
        
        int port_num, server_fd, conn_fd;
        struct sockaddr_in serv_addr, cli_addr;
        socklen_t cli_len;
        

        const std::map<std::string, Signals> commands = 
        {
            {"BUTTON_COMMAND_1", Signals::BUTTON_COMMAND_1},
            {"BUTTON_COMMAND_2", Signals::BUTTON_COMMAND_2},
            {"BUTTON_COMMAND_3", Signals::BUTTON_COMMAND_3},
            {"BUTTON_COMMAND_4", Signals::BUTTON_COMMAND_4},
            {"BUTTON_UP_1", Signals::BUTTON_UP_1},
            {"BUTTON_UP_2", Signals::BUTTON_UP_2},
            {"BUTTON_UP_3", Signals::BUTTON_UP_3},
            {"BUTTON_DOWN_2", Signals::BUTTON_DOWN_2},
            {"BUTTON_DOWN_3", Signals::BUTTON_DOWN_3},
            {"BUTTON_DOWN_4", Signals::BUTTON_DOWN_4},
            {"STOP_BUTTON", Signals::STOP_BUTTON},
            {"OBSTRUCTION", Signals::OBSTRUCTION},
            {"FLOOR_SENSOR", Signals::FLOOR_SENSOR},
            {"DOOR_LAMP", Signals::DOOR_LAMP},
            {"LIGHT_STOP", Signals::LIGHT_STOP},
            {"LIGHT_COMMAND_1", Signals::LIGHT_COMMAND_1},
            {"LIGHT_COMMAND_2", Signals::LIGHT_COMMAND_2},
            {"LIGHT_COMMAND_3", Signals::LIGHT_COMMAND_3},
            {"LIGHT_COMMAND_4", Signals::LIGHT_COMMAND_4},
            {"LIGHT_UP_1", Signals::LIGHT_UP_1},
            {"LIGHT_UP_2", Signals::LIGHT_UP_2},
            {"LIGHT_UP_3", Signals::LIGHT_UP_3},
            {"LIGHT_DOWN_2", Signals::LIGHT_DOWN_2},
            {"LIGHT_DOWN_3", Signals::LIGHT_DOWN_3},
            {"LIGHT_DOWN_4", Signals::LIGHT_DOWN_4},
            {"LIGHT_FLOOR_1", Signals::LIGHT_FLOOR_1},
            {"LIGHT_FLOOR_2", Signals::LIGHT_FLOOR_2},
            {"LIGHT_FLOOR_3", Signals::LIGHT_FLOOR_3},
            {"LIGHT_FLOOR_4", Signals::LIGHT_FLOOR_4},
            {"MOTOR_DIR", Signals::MOTOR_DIR},
            {"MOTOR_SPEED", Signals::MOTOR_SPEED}
        };
};


#endif //ELEV_SERVER_HPP
