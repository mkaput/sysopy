cmake_minimum_required(VERSION 3.6)
project(cw03)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES_ZAD1 zad1/main.c)
add_executable(zad1 ${SOURCE_FILES_ZAD1})

set(SOURCE_FILES_ZAD1_DUMP zad1/dump.c)
add_executable(zad1_dump ${SOURCE_FILES_ZAD1_DUMP})
add_dependencies(zad1 zad1_dump)

set(SOURCE_FILES_ZAD2 zad2/main.c)
add_executable(zad2 ${SOURCE_FILES_ZAD2})

set(SOURCE_FILES_ZAD2_BADBOY zad2/badboy.c)
add_executable(zad2_badboy ${SOURCE_FILES_ZAD2_BADBOY})
add_dependencies(zad2 zad1_dump)
add_dependencies(zad2 zad2_badboy)
