cmake_minimum_required(VERSION 3.7)
project(Lycoris)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(./LycorisNet/include)

set(SOURCE_FILES
        main.cpp
        LycorisNet/include/genome.h
        LycorisNet/sources/genome.cpp
        LycorisNet/include/node.h
        LycorisNet/sources/node.cpp
        LycorisNet/include/utils.h
        LycorisNet/sources/utils.cpp
        LycorisNet/include/individual.h
        LycorisNet/sources/individual.cpp
        LycorisNet/include/species.h
        LycorisNet/sources/species.cpp
        LycorisNet/include/lycoris.h
        LycorisNet/sources/lycoris.cpp
        LycorisNet/include/args.h
        LycorisNet/sources/args.cpp)

add_executable(bootstrap ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(bootstrap Threads::Threads)
