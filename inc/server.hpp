#ifndef ELEV_SERVER_HPP
#define ELEV_SERVER_HPP
/**
 * @file server.hpp
 * @author Oskar Oestby (oskar.oestby@gmail.com)
 * @brief A server for handling the client server interactions
 * @version 0.1
 * @date 2019-04-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

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

/**
 * @brief The elevator server class should handle all communication between the clients and the elevators.
 * 
 */
class ElevServer {
    public:
        /**
         * @brief Construct a new Elev Server object
         * 
         * @param port The port for the server socket.
         * @param num_of_elevators How many elevators you want.
         * @param log_file Path to a logfile.
         * @warning Currently the server can only handle one elevator.
         */
        ElevServer(int port, int num_of_elevators, std::string log_file);

        /**
         * @brief This functions starts the servers main loop.
         * 
         */
        void    run(void);

        /**
         * @brief Checks if the server is ok.
         * 
         * @return true The server is ok.
         * @return false The server is not ok.
         */
        bool    ok(void);

        /**
         * @brief If this function is called the server will stop.
         * 
         */
        void    stop(void);

        /**
         * @brief Get the position of the elevator with a specific ID.
         * 
         * @param id The id of the elevator that you want the position of. 
         * @return double The position of the elevator.
         */
        double  getPosition(int id);

        /**
         * @brief Send a message to the elevator.
         * 
         * @param msg The command message, for info see the README
         */
        void    elevControl(char unsigned msg[4]);
        
    private:

        /**
         * @brief Initialize the socket.
         * 
         */
        void        initSocket();

        /**
         * @brief Connect the socket to the given port and ip
         * 
         */
        void        connectSocket();
        
        /**
         * @brief Check if there is a socket connection ready.
         * 
         * @return int Value of the connection file descriptor. -1 for error, 0 for no connection.
         */
        int         selectSocket();

        /**
         * @brief Close the socket fd.
         * 
         */
        void        disconnectSocket();
        
        /**
         * @brief Stop all running elevators.
         * 
         */
        void        stopElevators();

        /**
         * @brief This is a function that handles connections. It is being run in the main loop.
         * 
         */
        void        handleConnections();

        /**
         * @brief Handle a single received message and return the appropriate response.
         * 
         * @param msg Message received from the connected client.
         * @return char* Response that should be sent back to the client.
         */
        char*       handleMessage(const unsigned char msg[4]);

        /**
         * @brief Parse the incomming message and create a command for the elevator.
         * 
         * @param msg The message to be parsed.
         * @return command_t Command to be sent to the elevator.
         */
        command_t   parseMessage(const unsigned char msg[4]);

        /**
         * @brief Send a command to the correct elevator and return its response.
         * 
         * @param cmd The command sent to the elevator.
         * @return command_t The response from the elevator.
         */
        command_t   executeCommand(const command_t &cmd);

        /**
         * @brief Create a response from the received command.
         * 
         * @param cmd Command to base the response on.
         * @return char* The response sent to the client.
         */
        char*       createResponse(const command_t &cmd);
        
        /**
         * @brief Start all elevator threads.
         * 
         */
        void runThreads();

        /**
         * @brief Join all elevator threads.
         * 
         */
        void joinThreads();

    private:
        /**
         * @brief A logger used for logging all kinds of info.
         * 
         */
        Logger logger;

        /**
         * @brief To indicate that the elevator is running.
         * 
         */
        bool running;
        
        /**
         * @brief Mutexes for handling connections and the ok status.
         * 
         */
        std::mutex conn_mtx, ok_mtx;
        
        /**
         * @brief A vector containing pointers to all running elevators.
         * 
         */
        std::vector< std::shared_ptr<elev::Elevator> > elevators;

        /**
         * @brief A thread pool holding all threads that are currently running.
         * 
         */
        std::vector<std::thread> thread_pool;
        
        int port_num, server_fd, conn_fd;
        struct sockaddr_in serv_addr, cli_addr;
        socklen_t cli_len;
};


#endif //ELEV_SERVER_HPP
