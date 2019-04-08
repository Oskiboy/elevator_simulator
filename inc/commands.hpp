#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>

enum class CommandType {
    GET= -1,
    SET=  1,
    ERROR = 0
};

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

enum class ButtonType {
    UP = 0,
    DOWN,
    COMMAND,
    NUM_BUTTONS
};

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

std::ostream& operator << (std::ostream& os, const command_t &cmd);

#endif //COMMANDS_HPP
