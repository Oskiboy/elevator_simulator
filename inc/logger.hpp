#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <fstream>

class Logger {
    public:
        Logger();
        Logger(std::string filename);
        ~Logger();
        void error(std::string msg);
        void info(std::string msg);
        void fatal(std::string msg);
    private:
        std::string generateTimestamp();
        std::string filename;
        std::ofstream file;
        time_t now;
        struct tm timestruct;
        char buffer[80];
};

#endif //LOGGER_HPP
