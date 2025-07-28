cmake_minimum_required(VERSION 3.16)
project(atmega328p-sim LANGUAGES C)

file(GLOB AVR_SRC compiler_main.c src/*.c)
add_executable(atm-cc "${AVR_SRC}")
target_include_directories(atm-cc PRIVATE include/)
