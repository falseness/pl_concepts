cmake_minimum_required(VERSION 3.13)
project(exceptions)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fno-exceptions)

add_executable(exceptions main.cpp)