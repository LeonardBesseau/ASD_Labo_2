cmake_minimum_required(VERSION 3.15)
project(ASD_Labo_2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(ASD_Labo_2 main.cpp pieces.cpp pieces.h)