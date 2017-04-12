cmake_minimum_required(VERSION 3.7)
project(cw06)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES zad1/main.c)
add_executable(zad1 ${SOURCE_FILES})

set(SOURCE_FILES2 zad1/main.c)
add_executable(zad2 ${SOURCE_FILES2})
