#include <server_commander.hpp>
#include <memory>
#include <signal.h>

std::unique_ptr<ServerCommander> c_ptr;

void abort_handler(int num) {
    c_ptr->stop();
}

int main(int argc, char* argv[]) {
    signal(SIGINT, abort_handler);
    signal(SIGABRT, abort_handler);

    c_ptr = std::make_unique<ServerCommander>(1, 4);
    c_ptr->run();
    return 0;
}