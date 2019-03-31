#ifndef ELEV_SERVER_HPP
#define ELEV_SERVER_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <map>

#include <elevator.hpp>
#include <logger.hpp>
#include <commands.hpp>

#include <sys/socket.h>
#include <netinet/in.h>

namespace elev {
class Elevator;
}

class ElevServer {
    public:
        ElevServer(int port, int num_of_elevators, std::string log_file);
        //~ElevServer();

        void    run(void);
        bool    ok(void);
        void    stop(void);
        double  getPosition(int id);
        void    elevControl(char msg[4]);
        
    private:
        void        initSocket();
        void        connectSocket();
        int         selectSocket();
        void        disconnectSocket();
        
        void        stopElevators();
        void        handleConnections();
        char*       handleMessage(const char msg[4]);
        command_t   parseMessage(const char msg[4]);
        command_t   executeCommand(const command_t &cmd);
        char*       createResponse(const command_t &cmd);
        
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
};


#endif //ELEV_SERVER_HPP
