cmake_minimum_required(VERSION 3.7)
project(Lycoris)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(SOURCE_FILES main.cpp)

add_executable(bootstrap ${SOURCE_FILES})