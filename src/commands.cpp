#include <commands.hpp>

std::ostream& operator << (std::ostream& os, const command_t &cmd) {
    os  << "CommandType:\t" << static_cast<int>(cmd.cmd) << std::endl
        << "Signal:\t\t" << static_cast<int>(cmd.signal) << std::endl
        << "Selector:\t" << cmd.selector << std::endl
        << "Floor:\t\t" << cmd.floor << std::endl
        << "Value:\t\t" << cmd.value << std::endl
        << "Elevator:\t" << cmd.elevator_num << std::endl
        << "Message:\t" << (int)cmd.msg[0] << "," << (int)cmd.msg[1] << "," <<(int) cmd.msg[2]<< "," << (int)cmd.msg[3]  << std::endl
        << "Position:\t" << cmd.position << std::endl;
    return os;
}