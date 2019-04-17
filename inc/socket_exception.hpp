#ifndef SOCKET_EXCEPTION_HPP
#define SOCKET_EXCEPTION_HPP

#include <stdexcept>

/**
 * @brief Thrown when the socket cannot bind.
 * 
 */
class SocketBindException: public std::runtime_error {
    public:
    SocketBindException(): std::runtime_error("Could not bind socket!") {}
};

/**
 * @brief Thrown when a socket could not be created.
 * 
 */
class SocketCreateException: public std::runtime_error {
    public:
    SocketCreateException(): std::runtime_error("Could not create socket!") {}
};

/**
 * @brief Thrown when a socket could not be accepted.
 * 
 */
class SocketAcceptException: public std::runtime_error {
    public:
    SocketAcceptException(): std::runtime_error("Could not accept connection!") {}
};

/**
 * @brief Thrown when the socket could not be read from.
 * 
 */
class SocketReadException: public std::runtime_error {
    public:
    SocketReadException(): std::runtime_error("Error while reading connection.") {}
};

/**
 * @brief Thrown when a socket could not be written to.
 * 
 */
class SocketWriteException: public std::runtime_error {
    public:
    SocketWriteException(): std::runtime_error("Error while writing to connection.") {}
};

#endif //SOCKET_EXCEPTION_HPP
