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
    CommandType     cmd;            ///< The type of command.
    CommandSignal   signal;         ///< The signal to be acted upon.
    int             selector;       ///< The selector, most often the floor or type of button
    int             floor;          ///< Which floor.
    int             value;          ///< What value should be set/is returned.
    int             elevator_num;   ///< The elevator id.
    double          position;       ///< The postion of the elevator.
    char            msg[4];         ///< The message the command was generated from.
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
