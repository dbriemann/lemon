#ifndef UTILITY_CONSTANTS_HPP
#define UTILITY_CONSTANTS_HPP

#include <string>
using namespace std;

#include "engine_constants.hpp"


static string COLORS[2] = {"White", "Black"};
static char PIECE_SYMBOLS[16] = {'.', 'P', 'N', 'K', '?', 'B', 'R', 'Q', '?',
                                 'p', 'n', 'k', '?', 'b', 'r', 'q'};
static char CHESS_COORDS[BOARD_SIZE] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};


#endif // UTILITY_CONSTANTS_HPP
