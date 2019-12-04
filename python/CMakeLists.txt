CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(LycorisNet)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(../LycorisNet/include)

SET(SOURCE_FILES
        ../LycorisNet/include/genome.h
        ../LycorisNet/sources/genome.cpp
        ../LycorisNet/include/node.h
        ../LycorisNet/sources/node.cpp
        ../LycorisNet/include/utils.h
        ../LycorisNet/sources/utils.cpp
        ../LycorisNet/include/individual.h
        ../LycorisNet/sources/individual.cpp
        ../LycorisNet/include/lycoris.h
        ../LycorisNet/sources/lycoris.cpp
        ../LycorisNet/include/args.h
        ../LycorisNet/sources/args.cpp
        LycorisNet.cpp)

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(pybind11 REQUIRED)

pybind11_add_module(LycorisNet ${SOURCE_FILES})
