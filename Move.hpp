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

#define FROM_MASK       0x000000ff //8bit
#define TO_MASK         0x0000ff00 //8bit (8)
#define PIECE_MASK      0x000f0000 //4bit (16)
#define CAPTURE_MASK    0x00f00000 //4bit (20)
#define PROMOTION_MASK  0x0f000000 //4bit (24)
#define EP_MASK         0x10000000 //1bit (28)
#define CASTLE_MASK     0x20000000 //1bit (29)
#define VALUE_MASK      0x40000000 //2bit (30)

#define FROM_SHIFT      0
#define TO_SHIFT        8
#define PIECE_SHIFT     16
#define CAPTURE_SHIFT   20
#define PROMOTION_SHIFT 24
#define EP_SHIFT        28
#define CASTLE_SHIFT    29
#define VALUE_SHIFT     30

inline Move moveCreate(U32 from, U32 to, U32 ptype, U32 captype, U32 promtype, U32 epflag, U32 castleflag, U32 value) {
    Move m = from;
    to <<= TO_SHIFT;
    ptype <<= PIECE_SHIFT;
    captype <<= CAPTURE_SHIFT;
    promtype <<= PROMOTION_SHIFT;
    epflag <<= EP_SHIFT;
    castleflag <<= CASTLE_SHIFT;
    value <<= VALUE_SHIFT;

    m |= (to | ptype | captype | promtype | epflag | castleflag | value);

    return m;
}


inline void moveSet(Move &m, const U32 mask, const U32 shift, U32 bits) {
    m &= ~mask;
    bits <<= shift;
    m |= bits;
}

inline U32 moveGet(Move &m, const U32 mask, const U32 shift) {
    return (m & mask) >> shift;
}

string moveToStr(Move m) {
    string ret;

    U32 from = moveGet(m, FROM_MASK, FROM_SHIFT);
    U32 to = moveGet(m, TO_MASK, TO_SHIFT);
    U32 ptype = moveGet(m, PIECE_MASK, PIECE_SHIFT);
    U32 cap = moveGet(m, CAPTURE_MASK, CAPTURE_SHIFT);
    U32 promo = moveGet(m, PROMOTION_MASK, PROMOTION_SHIFT);
    U32 epflag = moveGet(m, EP_MASK, EP_SHIFT);
    U32 castle = moveGet(m, CASTLE_MASK, CASTLE_SHIFT);
    U32 value = moveGet(m, VALUE_MASK, VALUE_SHIFT);


    if(castle) {
        //which type of castling
        if(to == CASTLE_SHORT_WHITE_TARGET || to == CASTLE_SHORT_BLACK_TARGET) {
            ret = "0-0";
        } else if(to == CASTLE_LONG_WHITE_TARGET || to == CASTLE_LONG_BLACK_TARGET) {
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
        if(cap != EMPTY) {
            ret += "x";
            //target piece if not pawn
            if(cap != PAWN) {
                ret += PIECE_SYMBOLS[cap & MASK_PIECE];
            }
        }
        //to
        xx = CHESS_COORDS[FILE(to)];
        yy = RANK(to) + 1;
        ret += xx;
        ret += intToString(yy);

        if(epflag) {
            ret+="ep";
        }

        if(promo != EMPTY) {
            ret += "=";
            ret += PIECE_SYMBOLS[promo & MASK_PIECE];
        }

        ret += "("; ret += intToString(value); ret += ")";
    }

    return ret;
}



#endif // MOVE_HPP
