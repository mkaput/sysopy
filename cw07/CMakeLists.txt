cmake_minimum_required(VERSION 3.7)
project(cw07)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES_ZAD1 zad1/main.c)
add_executable(zad1 ${SOURCE_FILES_ZAD1})
target_link_libraries(zad1 rt)

set(SOURCE_FILES_ZAD2 zad2/main.c)
add_executable(zad2 ${SOURCE_FILES_ZAD2})
target_link_libraries(zad2 rt pthread)
