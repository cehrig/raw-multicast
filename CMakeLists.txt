cmake_minimum_required(VERSION 3.5)
project(mining-pool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(SOURCE_FILES src/rmc.c)
add_executable(nemo ${SOURCE_FILES})