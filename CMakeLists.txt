project(lemon)

cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O3 -msse3 -g")

#header files compilation
set(HEADER_FILES BitBoard.hpp lemon.hpp utility_functions.hpp utility_constants.hpp Timers.hpp PerftData.hpp Move.hpp MoveList.hpp)

#liblemon dynamic library
add_library(liblemon SHARED ${HEADER_FILES})
set_target_properties(liblemon PROPERTIES LINKER_LANGUAGE CXX)

#engine executable
add_executable(lemon lemon.cpp)
target_link_libraries(lemon liblemon)

#tools executables
add_executable(gen_target_masks gen_target_masks.cpp)
target_link_libraries(gen_target_masks liblemon)

#perft executable
add_executable(perftbb perftbb.cpp)
target_link_libraries(perftbb liblemon)
add_executable(seriousperft seriousperft.cpp)
target_link_libraries(seriousperft liblemon)
