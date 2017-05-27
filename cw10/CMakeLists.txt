cmake_minimum_required(VERSION 3.7)
project(cw10)

set(CMAKE_C_STANDARD 99)

add_executable(zad1_serwer zad1/serwer.c zad1/task.h)
target_link_libraries(zad1_serwer pthread)
add_executable(zad1_klient zad1/klient.c zad1/task.h)

add_executable(zad2_serwer zad2/serwer.c zad2/task.h)
target_link_libraries(zad2_serwer pthread)
add_executable(zad2_klient zad2/klient.c zad2/task.h)
