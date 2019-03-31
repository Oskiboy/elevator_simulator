cmake_minimum_required(VERSION 3.0)
project(elevator_simulator)

include_directories(inc)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-std=c++14 -Wall -Werror -g3)

add_library(logger_lib "src/logger.cpp")
add_library(server_lib "src/server.cpp")
add_library(elev_lib "src/elevator.cpp")
add_library(command_lib "src/commands.cpp")
add_library(button_press_lib "src/button_press.cpp")

target_link_libraries(server_lib logger_lib pthread command_lib)
target_link_libraries(elev_lib command_lib button_press_lib)
add_executable(sim_server "src/main.cpp")
target_link_libraries(sim_server logger_lib server_lib elev_lib)