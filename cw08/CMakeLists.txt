cmake_minimum_required(VERSION 3.7)
project(cw08)

set(CMAKE_C_STANDARD 99)

add_executable(zad1a zad1/zad1a.c zad1/common.h)
target_link_libraries(zad1a pthread)
add_executable(zad1b zad1/zad1b.c zad1/common.h)
target_link_libraries(zad1b pthread)
add_executable(zad1c zad1/zad1c.c zad1/common.h)
target_link_libraries(zad1c pthread)
add_executable(mkf zad1/mkf.c)
target_link_libraries(mkf pthread)

add_executable(zad2a zad2/zad2a.c zad2/common.h)
target_link_libraries(zad2a pthread)
add_executable(zad2b zad2/zad2b.c zad2/common.h)
target_link_libraries(zad2b pthread)
