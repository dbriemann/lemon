#ifndef BB_DEFINITIONS_HPP
#define BB_DEFINITIONS_HPP

#include <cstdlib>
using namespace std;


/*
 * definitions
 */
#define BOARD_SIZE 8

#define WHITE 0
#define BLACK 1

#define NO_EN_PASSENT_IDX 0x80

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
#define MASK_PIECE      0x7

/*
 * type redefinitions
 */
typedef uint8_t U8; typedef int8_t I8;
typedef uint16_t U16; typedef int16_t I16;
typedef uint32_t U32; typedef int32_t I32;
typedef uint64_t U64; typedef int64_t I64;
#define C64(constantU64) constantU64##ULL
typedef U8 Index;
typedef U8 PieceType;
//typedef I8 Offset;




#endif // BB_DEFINITIONS_HPP