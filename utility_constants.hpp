#ifndef UTILITY_CONSTANTS_HPP
#define UTILITY_CONSTANTS_HPP

#include <string>
using namespace std;

#include "bb_definitions.hpp"


#define EMPTY     0x6

#define PAWN      0x0
#define KNIGHT    0x1
#define BISHOP    0x2
#define ROOK      0x3
#define QUEEN     0x4
#define KING      0x5
#define WHITE_PAWN      0x0
#define WHITE_KNIGHT    0x1
#define WHITE_BISHOP    0x2
#define WHITE_ROOK      0x3
#define WHITE_QUEEN     0x4
#define WHITE_KING      0x5
#define BLACK_PAWN      0x8
#define BLACK_KNIGHT    0x9
#define BLACK_BISHOP    0xA
#define BLACK_ROOK      0xB
#define BLACK_QUEEN     0xC
#define BLACK_KING      0xD
#define MASK_COLOR      0x8


static string COLORS[2] = {"White", "Black"};
static char PIECE_SYMBOLS[14] = {'P', 'N', 'B', 'R', 'Q', 'K', ' ', '?', 'p', 'n', 'b', 'r', 'q', 'k'};
static char CHESS_COORDS[BOARD_SIZE] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};


#endif // UTILITY_CONSTANTS_HPP
