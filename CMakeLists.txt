project(spirit_x3)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER   "clang")
set(CMAKE_CXX_COMPILER "clang++-3.6")
include(cmake/defaults.cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-shift-op-parentheses")

include_directories(include)

add_subdirectory(test/x3)

enable_testing()
