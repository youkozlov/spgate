cmake_minimum_required(VERSION 3.10)

# set the project name
project(spgate)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(app)

add_subdirectory(externals)

add_subdirectory(tests)

add_library(platform 
    rlcutil.cpp
    rlinifile.cpp
    rlsocket.cpp
    rlthread.cpp
    rlwthread.cpp
)

target_include_directories(app PUBLIC ./)

