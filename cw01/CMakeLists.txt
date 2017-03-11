cmake_minimum_required(VERSION 3.6)
project(cw1)

set(CMAKE_C_STANDARD 99)

set(ZAD1_SOURCE_FILES zad1/linked_list.c zad1/linked_list.h zad1/utils.c zad1/utils.h zad1/bst.c zad1/bst.h zad1/addrbook.c zad1/addrbook.h zad1/addr.c zad1/addr.h)
add_library(zad1 ${ZAD1_SOURCE_FILES})
