cmake_minimum_required(VERSION 3.7)
project(cw06)

set(CMAKE_C_STANDARD 99)

add_executable(zad1_serwer zad1/serwer.c)
add_executable(zad1_klient zad1/klient.c)

add_executable(zad2_serwer zad2/serwer.c)
add_executable(zad2_klient zad2/klient.c)
