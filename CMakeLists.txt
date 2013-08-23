project(lemon)

cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O3 -g")

#header files compilation
set(HEADER_FILES Board.hpp Board0x88.hpp board0x88_constants.hpp engine_constants.hpp utility_constants.hpp utility_functions.hpp
    Piece.hpp)

#liblemon dynamic library
add_library(liblemon SHARED ${HEADER_FILES})
set_target_properties(liblemon PROPERTIES LINKER_LANGUAGE CXX)

#engine executable
add_executable(lemon lemon.cpp)
target_link_libraries(lemon liblemon)
