cmake_minimum_required(VERSION 3.6)
project(addrbook)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES addrbook/linked_list.c addrbook/linked_list.h addrbook/utils.c addrbook/utils.h addrbook/bst.c addrbook/bst.h addrbook/addrbook.c addrbook/addrbook.h addrbook/addr.c addrbook/addr.h)
add_library(addrbook ${SOURCE_FILES})
