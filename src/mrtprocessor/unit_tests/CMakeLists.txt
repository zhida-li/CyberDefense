cmake_minimum_required(VERSION 3.26)
project(doctest_tests)

set(CMAKE_CXX_STANDARD 17)

add_executable(util_tests util_tests.cpp ../util.cpp)
