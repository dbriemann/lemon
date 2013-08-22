#ifndef ENGINE_CONSTANTS_HPP
#define ENGINE_CONSTANTS_HPP

#include <cstdint>
using namespace std;

//board info
static const uint8_t BOARD_SIZE = 8;
static const uint8_t INVALID_INDEX = 0x88;
static const uint8_t NO_EN_PASSENT_INDEX = 127;

//player/turn
static const uint8_t WHITE = 0;
static const uint8_t BLACK = 1;

//piece values on board
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



#endif // ENGINE_CONSTANTS_HPP
