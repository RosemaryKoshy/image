cmake_minimum_required(VERSION 3.17)
project(image C)

set(CMAKE_C_STANDARD 11)

add_executable(image image.c)
target_link_libraries(image m)