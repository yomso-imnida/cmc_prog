cmake_minimum_required(VERSION 3.22.1) # Последняя доступная версия CMake

project(ASVK)

set(CMAKE_CXX_STANDARD 23)

add_executable(ASVK
    Basic_Algorithm_Time.cpp
    read_info.cpp
    read_info.h
)