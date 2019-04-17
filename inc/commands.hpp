#ifndef COMMANDS_HPP
#define COMMANDS_HPP
/**
 * @file commands.hpp
 * @author Oskar Oestby (oskar.oestby@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <iostream>

/**
 * @brief Identifies what action a command should do.
 * 
 */
enum class CommandType {
    GET= -1,
    SET=  1,
    ERROR = 0
};


/**
 * @brief What signal should the command be used on.
 * 
 */
enum class CommandSignal {
    BUTTON,
    LIGHT,
    STOP,
    OBSTRUCTION,
    FLOOR_SENSOR,
    DOOR_LIGHT,
    STOP_LIGHT,
    MOTOR,
    POSITION,
    RESET,
    NUM_SIGNALS
};

/**
 * @brief The three types of buttons.
 * 
 */
enum class ButtonType {
    UP = 0,
    DOWN,
    COMMAND,
    NUM_BUTTONS
};

/**
 * @brief The command struct that is passed through the system.
 * This will be created by the server, executed by the elevator and then used by the server to create a response.
 * 
 */
typedef struct {
    CommandType     cmd;
    CommandSignal   signal;
    int             selector;
    int             floor;
    int             value;
    int             elevator_num;
    double          position;
    char            msg[4];
} command_t;

/**
 * @brief For easier printing of a command.
 * 
 * @param os Standard output stream.
 * @param cmd The command to be printed.
 * @return std::ostream& Standard output stream.
 */
std::ostream& operator << (std::ostream& os, const command_t &cmd);

#endif //COMMANDS_HPP
