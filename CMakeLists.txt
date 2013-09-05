project(lemon)

cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O3 -g")

#header files compilation
set(HEADER_FILES BitBoard.hpp bb_definitions.hpp utility_functions.hpp Timers.hpp PerftData.hpp)

#liblemon dynamic library
add_library(liblemon SHARED ${HEADER_FILES})
set_target_properties(liblemon PROPERTIES LINKER_LANGUAGE CXX)

#engine executable
#add_executable(lemon lemon.cpp)
#target_link_libraries(lemon liblemon)

#perft executable
#add_executable(perft0x88 perft0x88.cpp)
#target_link_libraries(perft0x88 liblemon)
add_executable(perftbb perftbb.cpp)
target_link_libraries(perftbb liblemon)
