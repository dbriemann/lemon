#ifndef BOARD0X88_CONSTANTS_HPP
#define BOARD0X88_CONSTANTS_HPP

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

static const uint8_t PAWN   = 0x01;
static const uint8_t KNIGHT = 0x02;
static const uint8_t KING   = 0x03;
static const uint8_t BISHOP = 0x05;
static const uint8_t ROOK   = 0x06;
static const uint8_t QUEEN  = 0x07;

//bitmasks
static const uint8_t MASK_RAW_PIECE_TYPE = 0x07;
static const uint8_t MASK_SLIDING_PIECE = 0x04;

//move types
static const uint8_t MOVETYPE_ORDINARY      = 0x0;
static const uint8_t MOVETYPE_CASTLE_SHORT  = 0x1;
static const uint8_t MOVETYPE_CASTLE_LONG   = 0x2;
static const uint8_t MOVETYPE_EN_PASSENT    = 0x3;
static const uint8_t MOVETYPE_PROMOTION_Q   = 0x4;
static const uint8_t MOVETYPE_PROMOTION_R   = 0x5;
static const uint8_t MOVETYPE_PROMOTION_B   = 0x6;
static const uint8_t MOVETYPE_PROMOTION_N   = 0x7;

//deltas for piece movement
static const int8_t QUEEN_DELTAS[8] = {15, 16, 17, -1, 1, -17, -16, -15};
static const int8_t ROOK_DELTAS[4]  = {16, -1, 1, -16};
static const int8_t BISHOP_DELTAS[4] = {15, 17, -17,  -15};
static const int8_t KNIGHT_DELTAS[8] = {14, 31, 33, 18, -14, -31, -33, -18};
static const int8_t KING_DELTAS[8] = {15, 16, 17, -1, 1, -17, -16, -15};
static const int8_t WHITE_PAWN_DELTAS[4] = {16, 32, 15, 17}; //advance one, two, capture left, capture right
static const int8_t BLACK_PAWN_DELTAS[4] = {-16, -32, -17, -15}; //advance one, two, capture left, capture right

#endif // BOARD0X88_CONSTANTS_HPP
