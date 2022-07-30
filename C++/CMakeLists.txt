cmake_minimum_required(VERSION 3.10)
project(Logger)

set(CMAKE_CXX_STANDARD 11)

include_directories(logger)
include_directories(test)

add_executable(logger_test
        logger/Logger.cpp
        logger/Logger.hpp
        test/main.cpp
        test/test.h
        test/utils.h
        test/BasicTest1.cpp test/ThreadTest1.cpp test/ThreadTest2.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(logger_test PRIVATE Threads::Threads)