cmake_minimum_required(VERSION 3.6)
project(cw04)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES zad1/main.c)
add_executable(zad1 ${SOURCE_FILES})

set(SOURCE_FILES2 zad2/main.c)
add_executable(zad2 ${SOURCE_FILES2})

set(SOURCE_FILES3 zad3/main.c)
add_executable(zad3 ${SOURCE_FILES3})
