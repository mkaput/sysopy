cmake_minimum_required(VERSION 3.6)
project(cw02)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES_ZAD1 zad1/main.c)
add_executable(zad1 ${SOURCE_FILES_ZAD1})

set(SOURCE_FILES_ZAD2 zad2/main.c)
add_executable(zad2 ${SOURCE_FILES_ZAD2})

set(SOURCE_FILES_ZAD3 zad3/main.c)
add_executable(zad3 ${SOURCE_FILES_ZAD3})
