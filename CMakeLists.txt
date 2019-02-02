cmake_minimum_required(VERSION 3.7)
project(Coroutine)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        coroutine.cpp
        coroutine.h
        co_task.cpp
        co_task.h
        main.cpp)

add_executable(Coroutine ${SOURCE_FILES})