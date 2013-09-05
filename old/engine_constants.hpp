#ifndef ENGINE_CONSTANTS_HPP
#define ENGINE_CONSTANTS_HPP

#include <cstdint>
using namespace std;


typedef uint8_t Index;
typedef int8_t Offset;

//board info
static const uint8_t BOARD_SIZE = 8;

//player/turn
static const uint8_t WHITE = 0;
static const uint8_t BLACK = 1;

static const uint8_t EMPTY        = 0x00;
static const uint8_t WHITE_PAWN   = 0x01;
static const uint8_t WHITE_KNIGHT = 0x02;
static const uint8_t WHITE_KING   = 0x03;
static const uint8_t WHITE_BISHOP = 0x05;
static const uint8_t WHITE_ROOK   = 0x06;
static const uint8_t WHITE_QUEEN  = 0x07;
static const uint8_t BLACK_PAWN   = 0x09;
static const uint8_t BLACK_KNIGHT = 0x0A;
static const uint8_t BLACK_KING   = 0x0B;
static const uint8_t BLACK_BISHOP = 0x0D;
static const uint8_t BLACK_ROOK   = 0x0E;
static const uint8_t BLACK_QUEEN  = 0x0F;

//move types
static const uint8_t MOVETYPE_ORDINARY      = 0x0;
static const uint8_t MOVETYPE_CASTLE_SHORT  = 0x1;
static const uint8_t MOVETYPE_CASTLE_LONG   = 0x2;
static const uint8_t MOVETYPE_EN_PASSENT    = 0x3;
static const uint8_t MOVETYPE_PROMOTION_Q   = 0x4;
static const uint8_t MOVETYPE_PROMOTION_R   = 0x5;
static const uint8_t MOVETYPE_PROMOTION_B   = 0x6;
static const uint8_t MOVETYPE_PROMOTION_N   = 0x7;

//bitmasks
static const uint8_t MASK_RAW_PIECE_TYPE = 0x07;
static const uint8_t MASK_SLIDING_PIECE = 0x04;
static const uint8_t MASK_COLOR = 0x08;
static const uint8_t MASK_PIECE = 0x07;

#endif // ENGINE_CONSTANTS_HPP
