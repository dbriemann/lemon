#ifndef BOARD0X88_CONSTANTS_HPP
#define BOARD0X88_CONSTANTS_HPP

typedef uint8_t Index;
typedef int8_t Offset;

static const uint8_t PAWN_DOUBLE_STEP_DIST = 32;
static const uint8_t INVALID_INDEX = 0x88;
static const uint8_t NO_EN_PASSENT = 127;
static const uint8_t DISABLE_SHORT_CASTLING = 0x1;
static const uint8_t DISABLE_LONG_CASTLING = 0x2;
static const uint8_t DISABLE_SHORT_OPP_CASTLING = 0x4;
static const uint8_t DISABLE_LONG_OPP_CASTLING = 0x8;
//static const uint8_t DISABLE_BOTH_CASTLING = 0x3;

//castling squares
static const Index CASTLE_SHORT_ROOK_PATH[2][2] = {{0x07, 0x05},{0x77, 0x75}};
static const Index CASTLE_SHORT_PATH[2][2] = {{0x05, 0x06}, {0x75, 0x76}};
static const Index CASTLE_SHORT_SQUARE[2] = {0x06, 0x76};
static const Index CASTLE_LONG_ROOK_PATH[2][2] = {{0x00, 0x03},{0x70, 0x73}};
static const Index CASTLE_LONG_PATH[2][2] = {{0x03, 0x02}, {0x73, 0x72}};
static const Index CASTLE_LONG_EXTRA_SQUARE[2] = {0x01, 0x71};
static const Index CASTLE_LONG_SQUARE[2] = {0x02, 0x72};

//pawn starting square range, for two step advance
static const Index PAWN_DOUBLE_STEP_RANGE[2][2] = {{0x10, 0x17}, {0x60, 0x67}};

//pawn promotion range
static const Index PAWN_PROMOTION_RANGE[2][2] = {{0x70, 0x77}, {0x00, 0x07}};

//color piece offset
static const uint8_t COLOR_PIECE_OFFSET[2] = {0x00, 0x08}; //white, black

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
static const uint8_t MASK_PIECE = 0x07;

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
static const Offset QUEEN_DELTAS[8] = {15, 16, 17, -1, 1, -17, -16, -15};
static const Offset ROOK_DELTAS[4]  = {16, -1, 1, -16};
static const Offset BISHOP_DELTAS[4] = {15, 17, -17,  -15};
static const Offset KNIGHT_DELTAS[8] = {14, 31, 33, 18, -14, -31, -33, -18};
static const Offset KING_DELTAS[8] = {15, 16, 17, -1, 1, -17, -16, -15};
static const Offset PAWN_DELTAS[2][3] = {{16, 15, 17}, {-16, -17, -15}}; //advance one, two, capture left, capture right //advance one, two, capture left, capture right

//piece values for attack table
//EMPTY, WHITE_PAWN, KNIGHT, KING, _, BISHOP, ROOK, QUEEN, _, BLACK_PAWN
static char PIECE_ATTACK_MASK[10] = {0, 0x20, 0x2, 0x4, 0, 0x1, 0x8, 0x10, 0, 0x40};

//pieces able to attack
static const uint8_t ATTACK_NONE = 0;
static const uint8_t ATTACK_KQR = 0x1C;//1;
static const uint8_t ATTACK_QR = 0x18;//2;
static const uint8_t ATTACK_KQBwP = 0x35;//3;
static const uint8_t ATTACK_KQBbP = 0x55;//4;
static const uint8_t ATTACK_QB = 0x11;//5;
static const uint8_t ATTACK_N = 0x2;//6;

// Formula: attacked_square - attacking_square + 128 = pieces able to attack

static const uint8_t ATTACK_ARRAY[257] =
    {0,0,0,0,0,0,0,0,0,0x11,0,0,0,0,0,0,0x18,0,0,0,                                         //0-19
     0,0,0,0x11,0,0,0x11,0,0,0,0,0,0x18,0,0,0,0,0,0x11,0,                                   //20-39
     0,0,0,0x11,0,0,0,0,0x18,0,0,0,0,0x11,0,0,0,0,0,0,                                      //40-59
     5,0,0,0,0x18,0,0,0,0x11,0,0,0,0,0,0,0,0,0x11,0,0,                                      //60-79
     0x18,0,0,0x11,0,0,0,0,0,0,0,0,0,0,0x11,0x2,0x18,0x2,0x11,0,                            //80-99
     0,0,0,0,0,0,0,0,0,0,0x2,0x55,0x1C,0x55,0x2,0,0,0,0,0,                                  //100-119
     0,0x18,0x18,0x18,0x18,0x18,0x18,0x1C,0,0x1C,0x18,0x18,0x18,0x18,0x18,0x18,0,0,0,0,     //120-139
     0,0,0x2,0x35,0x1C,0x35,0x2,0,0,0,0,0,0,0,0,0,0,0,0x11,0x2,                             //140-159
     0x18,0x2,0x11,0,0,0,0,0,0,0,0,0,0,0x11,0,0,0x18,0,0,0x11,                              //160-179
     0,0,0,0,0,0,0,0,0x11,0,0,0,0x18,0,0,0,0x11,0,0,0,                                      //180-199
     0,0,0,0x11,0,0,0,0,0x18,0,0,0,0,0x11,0,0,0,0,0x11,0,                                   //200-219
     0,0,0,0,0x18,0,0,0,0,0,0x11,0,0,0x11,0,0,0,0,0,0,                                      //220-239
     0x18,0,0,0,0,0,0,0x11,0,0,0,0,0,0,0,0,0         };                                     //240-256


// Same as attack array but gives the delta needed to get to the square
static const Offset DELTA_ARRAY[257] =
{  0,   0,   0,   0,   0,   0,   0,   0,   0, -17,   0,   0,   0,   0,   0,   0, -16,   0,   0,   0,
   0,   0,   0, -15,   0,   0, -17,   0,   0,   0,   0,   0, -16,   0,   0,   0,   0,   0, -15,   0,
   0,   0,   0, -17,   0,   0,   0,   0, -16,   0,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,
 -17,   0,   0,   0, -16,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0, -17,   0,   0,
 -16,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -17, -33, -16, -31, -15,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -18, -17, -16, -15, -14,   0,   0,   0,   0,   0,
   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,
   0,   0,  14,  15,  16,  17,  18,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  15,  31,
  16,  33,  17,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,  16,   0,   0,  17,
   0,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,  16,   0,   0,   0,  17,   0,   0,   0,
   0,   0,   0,  15,   0,   0,   0,   0,  16,   0,   0,   0,   0,  17,   0,   0,   0,   0,  15,   0,
   0,   0,   0,   0,  16,   0,   0,   0,   0,   0,  17,   0,   0,  15,   0,   0,   0,   0,   0,   0,
  16,   0,   0,   0,   0,   0,   0,  17,   0,   0,   0,   0,   0,   0,   0,   0,   0};

#endif // BOARD0X88_CONSTANTS_HPP
