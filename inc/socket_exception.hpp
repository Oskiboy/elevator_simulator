#ifndef SOCKET_EXCEPTION_HPP
#define SOCKET_EXCEPTION_HPP

#include <stdexcept>

class SocketBindException: public std::runtime_error {
    public:
    SocketBindException(): std::runtime_error("Could not bind socket!") {}
};

class SocketCreateException: public std::runtime_error {
    public:
    SocketCreateException(): std::runtime_error("Could not create socket!") {}
};
class SocketAcceptException: public std::runtime_error {
    public:
    SocketAcceptException(): std::runtime_error("Could not accept connection!") {}
};

class SocketReadException: public std::runtime_error {
    public:
    SocketReadException(): std::runtime_error("Error while reading connection.") {}
};

class SocketWriteException: public std::runtime_error {
    public:
    SocketWriteException(): std::runtime_error("Error while writing to connection.") {}
};

#endif //SOCKET_EXCEPTION_HPP
