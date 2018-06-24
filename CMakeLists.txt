cmake_minimum_required(VERSION 3.5)
project(raw-multicast)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/rmc.c src/in.c src/out.c src/log.c src/dispatch.c)
add_executable(rmc ${SOURCE_FILES})