cmake_minimum_required(VERSION 3.10)
project(Logger)

set(CMAKE_C_STANDARD 11)

include_directories(logger)
include_directories(test)

add_executable(logger_test
        logger/logger.c
        logger/logger.h
        test/main.c test/utils.h test/test.h test/BasicTest1.c test/ThreadTest1.c test/ThreadTest2.c)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(logger_test PRIVATE Threads::Threads)