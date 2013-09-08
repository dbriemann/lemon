#ifndef MOVE_HPP
#define MOVE_HPP

#include "lemon.hpp"
#include "utility_functions.hpp"

/*
 * Moves are U32
 */
typedef U32 Move;

#define MOVE_PRE_EVAL_BADCAP    0
#define MOVE_PRE_EVAL_DEFAULT   1
#define MOVE_PRE_EVAL_GOODCAP   2
#define MOVE_PRE_EVAL_CASTLE    3
#define MOVE_PRE_EVAL_PROMO     4
#define MOVE_PRE_EVAL_PROMOCAP  5
//TODO.. capture evals without conditionals in movegen..

#define EP_TYPE_NONE    0
#define EP_TYPE_CAPTURE 1
#define EP_TYPE_CREATE  2

#define CAPTURE_YES 0x1
#define CAPTURE_NO  0x0

#define CASTLE_YES  0x1
#define CASTLE_NO   0x0

static const U64 CASTLE_SHORT_PATH[2] = {0x00000060, 0x60000000};
static const U64 CASTLE_LONG_PATH[2] = {0x0000000E, 0xE0000000};
static const U8 CASTLE_SHORT_TARGET[2] = {6,62};
static const U8 CASTLE_LONG_TARGET[2] = {2,58};

#define FROM_MASK       0x000000FF //8bit
#define TO_MASK         0x0000FF00 //8bit (8)
#define PIECE_MASK      0x000F0000 //4bit (16)
#define PROMOTION_MASK  0x00F00000 //4bit (20)
#define EP_MASK         0x03000000 //2bit (24)
#define CAPTURE_MASK    0x04000000 //1bit (26)
#define CASTLE_MASK     0x08000000 //1bit (27)
#define VALUE_MASK      0xF0000000 //4bit (28)

#define FROM_SHIFT      0
#define TO_SHIFT        8
#define PIECE_SHIFT     16
#define PROMOTION_SHIFT 20
#define EP_SHIFT        24
#define CAPTURE_SHIFT   26
#define CASTLE_SHIFT    27
#define VALUE_SHIFT     28

inline Move moveCreate(U32 from, U32 to, U32 ptype, U32 capflag, U32 promtype, U32 eptype, U32 castleflag, U32 value) {
    Move m = from;
    to <<= TO_SHIFT;
    ptype <<= PIECE_SHIFT;    
    promtype <<= PROMOTION_SHIFT;
    eptype <<= EP_SHIFT;
    capflag <<= CAPTURE_SHIFT;
    castleflag <<= CASTLE_SHIFT;
    value <<= VALUE_SHIFT;

    m |= (to | ptype | promtype | eptype | capflag | castleflag | value);

    return m;
}


inline void moveSetFeature(Move &m, const U32 mask, const U32 shift, U32 bits) {
    m &= ~mask;
    bits <<= shift;
    m |= bits;
}

inline U32 moveGetFeature(Move &m, const U32 mask, const U32 shift) {
    return (m & mask) >> shift;
}

string moveToStr(Move m) {
    string ret = "";

    U32 from = moveGetFeature(m, FROM_MASK, FROM_SHIFT);
    U32 to = moveGetFeature(m, TO_MASK, TO_SHIFT);
    U32 ptype = moveGetFeature(m, PIECE_MASK, PIECE_SHIFT);
    U32 cap = moveGetFeature(m, CAPTURE_MASK, CAPTURE_SHIFT);
    U32 promo = moveGetFeature(m, PROMOTION_MASK, PROMOTION_SHIFT);
    U32 epflag = moveGetFeature(m, EP_MASK, EP_SHIFT);
    U32 castle = moveGetFeature(m, CASTLE_MASK, CASTLE_SHIFT);
    U32 value = moveGetFeature(m, VALUE_MASK, VALUE_SHIFT);


    if(castle) {
        //which type of castling
        if(to == CASTLE_SHORT_TARGET[WHITE] || to == CASTLE_SHORT_TARGET[BLACK]) {
            ret = "0-0";
        } else if(to == CASTLE_LONG_TARGET[WHITE] || to == CASTLE_LONG_TARGET[BLACK]) {
            ret = "0-0-0";
        }
    } else {
        //piece if not pawn
        if(ptype != PAWN) {
            ret += PIECE_SYMBOLS[ptype & MASK_PIECE];
        }
        //from
        char xx = CHESS_COORDS[FILE(from)];
        uint8_t yy = RANK(from) + 1;
        ret += xx;
        ret += intToString(yy);
        //x if capture
        if(cap) {
            ret += "x";
        } else {
            ret += "-";
        }
        //to
        xx = CHESS_COORDS[FILE(to)];
        yy = RANK(to) + 1;
        ret += xx;
        ret += intToString(yy);

        if(epflag == EP_TYPE_CAPTURE) {
            ret+=" ep";
        }

        if(promo != EMPTY) {
            ret += "=";
            ret += PIECE_SYMBOLS[promo & MASK_PIECE];
        }
    }
    ret += " ("; ret += intToString(value); ret += ")";

    return ret;
}



#endif // MOVE_HPP
