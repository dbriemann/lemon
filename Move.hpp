#ifndef MOVE_HPP
#define MOVE_HPP

#include <cstdint>
#include <string>
using namespace std;

#include "board0x88_constants.hpp"
#include "utility_constants.hpp"
#include "utility_functions.hpp"


typedef uint32_t Move;

static void moveMake(Move &m, uint32_t ptype, uint32_t from, uint32_t to, uint32_t mtype, uint32_t capture) {
    m = 0;
    ptype <<= 28; //4 bits
    from <<= 21; //7 bits
    to <<= 14; //7 bits
    mtype <<= 11; //3 bits
    capture <<= 7; //4 bits;
    //still 7 bits unused --> move eval for move ordering?!

    m = ptype | from | to | mtype | capture;
}

static uint8_t moveGetPType(Move m) {
    m >>= 28;
    return static_cast<uint8_t>(m);
}

static uint8_t moveGetFrom(Move m) {
    m >>= 21;
    m = m & (0x7F);
    return static_cast<uint8_t>(m);
}

static uint8_t moveGetTo(Move m) {
    m >>= 14;
    m = m & (0x7F);
    return static_cast<uint8_t>(m);
}

static uint8_t moveGetMType(Move m) {
    m >>= 11;
    m = m & (0x7);
    return static_cast<uint8_t>(m);
}

static uint8_t moveGetCapture(Move m) {
    m >>= 7;
    m = m & (0xF);
    return static_cast<uint8_t>(m);
}

static string moveToString(Move m) {
    uint8_t ptype = moveGetPType(m);
    uint8_t from = moveGetFrom(m);
    uint8_t to = moveGetTo(m);
    uint8_t mtype = moveGetMType(m);
    uint8_t capture = moveGetCapture(m);

    string ret;

    if(mtype == MOVETYPE_CASTLE_SHORT) {
        ret = "0-0";
    } else if(mtype == MOVETYPE_CASTLE_LONG) {
        ret = "0-0-0";
    } else {
        //piece if not pawn
        if(ptype != BLACK_PAWN && ptype != WHITE_PAWN) {
            ret += PIECE_SYMBOLS[ptype];
            //ret += intToString(ptype);
        }
        //from
        char xx = CHESS_COORDS[from % 16];
        uint8_t yy = (from / 16) + 1;
        ret += xx;
        ret += intToString(yy);
        //x if capture
        if(capture != EMPTY) {
            ret+="x";
            //target piece if not pawn
            if(capture != WHITE_PAWN && capture != BLACK_PAWN) {
                ret += PIECE_SYMBOLS[capture];
            }
        }
        //to
        xx = CHESS_COORDS[to % 16];
        yy = (to / 16) + 1;
        ret += xx;
        ret += intToString(yy);

        if(mtype == MOVETYPE_EN_PASSENT) {
            ret+="ep";
        } else if(mtype == MOVETYPE_PROMOTION_Q) {
            ret+="=Q";
        } else if(mtype == MOVETYPE_PROMOTION_R) {
            ret+="=R";
        } else if(mtype == MOVETYPE_PROMOTION_B) {
            ret+="=B";
        } else if(mtype == MOVETYPE_PROMOTION_N) {
            ret+="=N";
        }
    }

    return ret;
}


#endif // MOVE_HPP
