
#include <logger.hpp>
#include <ctime>


Logger::Logger(std::string filename):
filename(filename)
{
    file.open(filename, std::ios_base::app | std::ios_base::out);
    if(!file.is_open()) {
        error("Could not open logile");
    }
    info("Logger initiated...");
}

Logger::Logger():
Logger("logs.log")
{
    //Empty
}

Logger::~Logger() {
    file.close();
}

std::string Logger::generateTimestamp() {
    now = std::time(0);
    timestruct = *localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &timestruct);
    return std::string(buffer);
}

void Logger::info(std::string msg) {
    file << "\033[32m[" << generateTimestamp() << "][INFO]\033[39m - " << msg << std::endl;
}

void Logger::error(std::string msg) {
    file << "\033[33m[" << generateTimestamp() << "][ERROR]\033[39m - " << msg << std::endl;
}

void Logger::fatal(std::string msg) {
    file << "\033[31m[" << generateTimestamp() << "][FATAL]\033[39m - " << msg << std::endl;
}
