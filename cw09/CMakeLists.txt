cmake_minimum_required(VERSION 3.7)
project(cw09)

set(CMAKE_C_STANDARD 99)

add_executable(zad1 zad1/main.c)
target_link_libraries(zad1 pthread)

add_executable(zad2 zad2/main.c)
target_link_libraries(zad2 pthread)
