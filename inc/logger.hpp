#ifndef LOGGER_HPP
#define LOGGER_HPP
/**
 * @file logger.hpp
 * @author Oskar Oestby (oskar.oestby@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <string>
#include <fstream>

class Logger {
    public:
        /**
         * @brief Construct a new Logger object
         * 
         */
        Logger();

        /**
         * @brief Construct a new Logger object
         * 
         * @param filename The path to where the logfile is wanted.
         */
        Logger(std::string filename);

        /**
         * @brief Destroy the Logger object
         * 
         */
        ~Logger();

        /**
         * @brief Log an error.
         * 
         * @param msg The error message to be logged.
         */
        void error(std::string msg);

        /**
         * @brief Log an info message.
         * 
         * @param msg The info message to be logged.
         */
        void info(std::string msg);

        /**
         * @brief Log a fatal message.
         * 
         * @param msg The message to be logged.
         */
        void fatal(std::string msg);
    private:

        /**
         * @brief Helper function to generate a timestamp of the current time.
         * 
         * @return std::string Timestamp in ISO format.
         */
        std::string generateTimestamp();
        std::string filename;
        std::ofstream file;
        time_t now;
        struct tm timestruct;
        char buffer[80];        ///A buffer for creating timestamps.
};

#endif //LOGGER_HPP
