CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(Lycoris)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/LycorisNet/lib)

INCLUDE_DIRECTORIES(./LycorisNet/include)

SET(SOURCE_FILES
        LycorisNet/include/genome.h
        LycorisNet/sources/genome.cpp
        LycorisNet/include/node.h
        LycorisNet/sources/node.cpp
        LycorisNet/include/utils.h
        LycorisNet/sources/utils.cpp
        LycorisNet/include/individual.h
        LycorisNet/sources/individual.cpp
        LycorisNet/include/lycoris.h
        LycorisNet/sources/lycoris.cpp
        LycorisNet/include/args.h
        LycorisNet/sources/args.cpp)

ADD_LIBRARY(lycoris SHARED ${SOURCE_FILES})

FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(lycoris Threads::Threads)

INSTALL(TARGETS lycoris
        LIBRARY DESTINATION lib)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LycorisNet/include/args.h DESTINATION include)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LycorisNet/include/genome.h DESTINATION include)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LycorisNet/include/individual.h DESTINATION include)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LycorisNet/include/lycoris.h DESTINATION include)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LycorisNet/include/node.h DESTINATION include)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LycorisNet/include/utils.h DESTINATION include)
