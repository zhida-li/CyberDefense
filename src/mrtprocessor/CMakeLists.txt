cmake_minimum_required(VERSION 3.26)
project(mrtprocessor)

set(CMAKE_CXX_STANDARD 17)

include_directories("/Users/ballanty/.local/include")

add_executable(mrtprocessor main.cpp
        summarydata.cpp
        summarydata.h
        BS_thread_pool.hpp)
target_link_directories(mrtprocessor PUBLIC "/Users/ballanty/.local/lib")
target_link_libraries(mrtprocessor "-lbgpdump" "-lbz2" "-lz")
