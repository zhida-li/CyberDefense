cmake_minimum_required(VERSION 3.26)
project(mrtprocessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_ARCHITECTURES "arm64")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-check")

include_directories("/Users/ballanty/.local/include")

add_executable(mrtprocessor main.cpp
        summarydata.cpp
        summarydata.h
        BS_thread_pool.hpp
        multibgpreader.cpp
        multibgpreader.h)
target_link_directories(mrtprocessor PUBLIC "/Users/ballanty/.local/lib")
target_link_libraries(mrtprocessor "-lbgpdump" "-lbz2" "-lz")
