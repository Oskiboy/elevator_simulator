cmake_minimum_required(VERSION 3.0)
project(elevator_sim_wrapper)

add_compile_options(-O0 -g3 -Wall -Werror -std=gnu99)

if(TTK_SIM)
    add_compile_options(-DTTK_SIM)
endif()

add_library(elev_wrapper "elev.c")