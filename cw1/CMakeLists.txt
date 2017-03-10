cmake_minimum_required(VERSION 3.6)
project(sysopy_ksiazka_adresowa)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c linked_list.c linked_list.h utils.c utils.h bst.c bst.h)
add_executable(sysopy_ksiazka_adresowa ${SOURCE_FILES})