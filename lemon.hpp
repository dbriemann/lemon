#ifndef BB_DEFINITIONS_HPP
#define BB_DEFINITIONS_HPP

#include <cstdlib>
using namespace std;


/*
 * Types
 */
typedef uint8_t U8; typedef int8_t I8;
typedef uint16_t U16; typedef int16_t I16;
typedef uint32_t U32; typedef int32_t I32;
typedef uint64_t U64; typedef int64_t I64;
typedef U8 Index;
//typedef I8 Offset;
#define C64(constantU64) constantU64##ULL
#define iBitMask(i) (1ULL << (i))


/*
 * Boad & Piece definitions
 */
#define BOARD_SIZE                  8
#define CASTLE_SHORT_WHITE_TARGET   6
#define CASTLE_SHORT_BLACK_TARGET   62
#define CASTLE_LONG_WHITE_TARGET    2
#define CASTLE_LONG_BLACK_TARGET    58
#define MASK_COLOR                  0x8
#define MASK_PIECE                  0x7
#define FLIP(x) (((x)+1)%2)

#define NONE 0

enum Color {
    WHITE, BLACK
};

enum PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum OccupancyType {
    WPAWN, WKNIGHT, WBISHOP, WROOK, WQUEEN, WKING, EMPTY,
    BPAWN=8, BKNIGHT, BBISHOP, BROOK, BQUEEN, BKING
};


/*
 * Movement definitions
 */
/*
 *northwest    north   northeast
  NW             N            NE

          +7    +8    +9
              \  |  /
  W       -1 <-  0 -> +1       E
              /  |  \
          -9    -8    -7

  SW             S            SE
  southwest    south   southeast
 *
 */
#define _SHIFT_N(x) ((x) << 8)
#define _SHIFT_NN(x) ((x) << 16) //pawn double step
#define _SHIFT_S(x) ((x) >> 8)
#define _SHIFT_SS(x) ((x) >> 16) //pawn double step
#define _SHIFT_W(x) ((x) >> 1)
#define _SHIFT_E(x) ((x) << 1)
#define _SHIFT_NE(x) ((x) << 9)
#define _SHIFT_SE(x) ((x) >> 7)
#define _SHIFT_SW(x) ((x) >> 9)
#define _SHIFT_NW(x) ((x) << 7)
//knight's jumps
#define _SHIFT_NNE(x) ((x) << 17)
#define _SHIFT_NNW(x) ((x) << 15)
#define _SHIFT_SSE(x) ((x) >> 15)
#define _SHIFT_SSW(x) ((x) >> 17)
#define _SHIFT_WNW(x) ((x) << 6)
#define _SHIFT_WSW(x) ((x) >> 10)
#define _SHIFT_ENE(x) ((x) << 10)
#define _SHIFT_ESE(x) ((x) >> 6)

static const int PAWN_MOVE_DIRECTIONS[2] = {8, -8};
static const int PAWN_CAP_EAST_DIRECTIONS[2] = {9, -7};
static const int PAWN_CAP_WEST_DIRECTIONS[2] = {7, -9};


/*
 * Rank & file definitions
 */
#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL


#define SQ(f,r) (((r) << 3) | (f))
#define FILE(sq) ((sq) &  7)
#define RANK(sq) ((sq) >> 3)


/*
 * Movegen target tables
 */
static const U64 KNIGHT_TARGET_BBS[64] = {
    0x20400ULL, 0x50800ULL, 0xa1100ULL, 0x142200ULL, 0x284400ULL, 0x508800ULL, 0xa01000ULL, 0x402000ULL,
    0x2040004ULL, 0x5080008ULL, 0xa110011ULL, 0x14220022ULL, 0x28440044ULL, 0x50880088ULL, 0xa0100010ULL, 0x40200020ULL,
    0x204000402ULL, 0x508000805ULL, 0xa1100110aULL, 0x1422002214ULL, 0x2844004428ULL, 0x5088008850ULL, 0xa0100010a0ULL, 0x4020002040ULL,
    0x20400040200ULL, 0x50800080500ULL, 0xa1100110a00ULL, 0x142200221400ULL, 0x284400442800ULL, 0x508800885000ULL, 0xa0100010a000ULL, 0x402000204000ULL,
    0x2040004020000ULL, 0x5080008050000ULL, 0xa1100110a0000ULL, 0x14220022140000ULL, 0x28440044280000ULL, 0x50880088500000ULL, 0xa0100010a00000ULL, 0x40200020400000ULL,
    0x204000402000000ULL, 0x508000805000000ULL, 0xa1100110a000000ULL, 0x1422002214000000ULL, 0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 0x4020002040000000ULL,
    0x400040200000000ULL, 0x800080500000000ULL, 0x1100110a00000000ULL, 0x2200221400000000ULL, 0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010a000000000ULL, 0x2000204000000000ULL,
    0x4020000000000ULL, 0x8050000000000ULL, 0x110a0000000000ULL, 0x22140000000000ULL, 0x44280000000000ULL, 0x88500000000000ULL, 0x10a00000000000ULL, 0x20400000000000ULL
};


/*
 * Bitscan & popcount
 */
const U32 index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};
#define DEBRUIJN64 0x03f79d71b4cb0a89ULL

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
U32 bitscanfwd(U64 bb) {
   return index64[((bb ^ (bb-1)) * DEBRUIJN64) >> 58];
}




#endif // BB_DEFINITIONS_HPP
