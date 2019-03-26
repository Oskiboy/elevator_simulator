cmake_minimum_required(VERSION 3.0)
project(elevator_simulator)

include_directories(inc)
add_compile_options(-std=c++14 -Wall -Werror -g3)

add_library(logger_lib "src/logger.cpp")
add_library(server_lib "src/server.cpp")
add_library(elev_lib "src/elevator.cpp")

target_link_libraries(server_lib logger_lib pthread)

add_executable(sim_server "src/main.cpp")
target_link_libraries(sim_server logger_lib server_lib elev_lib)