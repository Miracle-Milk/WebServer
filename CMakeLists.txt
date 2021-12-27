cmake_minimum_required(VERSION 3.21)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(WebServer main.cpp ThreadPool.cpp ThreadPool.h)
