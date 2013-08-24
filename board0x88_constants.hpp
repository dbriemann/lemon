#ifndef BOARD0X88_CONSTANTS_HPP
#define BOARD0X88_CONSTANTS_HPP

typedef int8_t Index;

//castling squares
static const Index WHITE_CASTLE_SHORT_PATH[2] = {0x05,0x06};
static const Index WHITE_CASTLE_SHORT_SQUARE = 0x06;
static const Index WHITE_CASTLE_LONG_PATH[2] = {0x03,0x02};
static const Index WHITE_CASTLE_LONG_SQUARE = 0x02;
static const Index BLACK_CASTLE_SHORT_PATH[2] = {0x75,0x76};
static const Index BLACK_CASTLE_SHORT_SQUARE = 0x76;
static const Index BLACK_CASTLE_LONG_PATH[2] = {0x73,0x72};
static const Index BLACK_CASTLE_LONG_SQUARE = 0x72;

//pawn starting square range, for two step advance
static const Index PAWN_DOUBLE_JUMP_RANGE[2][2] = {{0x10, 0x17}, {0x60, 0x67}};

//pawn promotion range
static const Index PAWN_PROMOTION_RANGE[2][2] = {{0x70, 0x77}, {0x00, 0x07}};

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

static const uint8_t PAWN   = WHITE_PAWN;
static const uint8_t KNIGHT = WHITE_KNIGHT;
static const uint8_t KING   = WHITE_KING;
static const uint8_t BISHOP = WHITE_BISHOP;
static const uint8_t ROOK   = WHITE_ROOK;
static const uint8_t QUEEN  = WHITE_QUEEN;

//bitmasks
static const uint8_t MASK_RAW_PIECE_TYPE = 0x07;
static const uint8_t MASK_SLIDING_PIECE = 0x04;
static const uint8_t MASK_COLOR = 0x08;

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
static const Index QUEEN_DELTAS[8] = {15, 16, 17, -1, 1, -17, -16, -15};
static const Index ROOK_DELTAS[4]  = {16, -1, 1, -16};
static const Index BISHOP_DELTAS[4] = {15, 17, -17,  -15};
static const Index KNIGHT_DELTAS[8] = {14, 31, 33, 18, -14, -31, -33, -18};
static const Index KING_DELTAS[8] = {15, 16, 17, -1, 1, -17, -16, -15};
static const Index PAWN_DELTAS[2][3] = {{16, 15, 17}, {-16, -17, -15}}; //advance one, two, capture left, capture right //advance one, two, capture left, capture right

#endif // BOARD0X88_CONSTANTS_HPP
