cmake_minimum_required(VERSION 3.16)
project(atmega328p-sim LANGUAGES C)

file(GLOB AVR_SRC src/*.c)
add_executable(atmega328p-sim "${AVR_SRC}")
target_include_directories(atmega328p-sim PRIVATE include/)
