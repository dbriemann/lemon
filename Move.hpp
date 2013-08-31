#ifndef MOVE_HPP
#define MOVE_HPP

#include <cstdint>
#include <string>
using namespace std;

#include "board0x88_constants.hpp"
#include "utility_constants.hpp"
#include "utility_functions.hpp"


struct Move {
    uint8_t piece_type;
    uint8_t from;
    uint8_t to;
    uint8_t move_type;
    uint8_t capture_type;
    uint8_t castling_mask;

    Move();
    Move(uint8_t ptype, uint8_t from, uint8_t to, uint8_t mtype, uint8_t ctype);

    string toString();
};

Move::Move() {
    piece_type = EMPTY;
    from = 255; //may crash
    to = 255; //may crash
    move_type = MOVETYPE_ORDINARY;
    capture_type = EMPTY;
    castling_mask = 0x0;
}

Move::Move(uint8_t ptype, uint8_t from, uint8_t to, uint8_t mtype, uint8_t ctype) {
    piece_type = ptype;
    this->from = from;
    this->to = to;
    move_type = mtype;
    capture_type = ctype;
    castling_mask = 0x0;
}

string Move::toString() {
    string ret;

    if(move_type == MOVETYPE_CASTLE_SHORT) {
        ret = "0-0";
    } else if(move_type == MOVETYPE_CASTLE_LONG) {
        ret = "0-0-0";
    } else {
        //piece if not pawn
        if(piece_type != BLACK_PAWN && piece_type != WHITE_PAWN) {
            ret += PIECE_SYMBOLS[piece_type & MASK_PIECE];
            //ret += intToString(ptype);
        }
        //from
        char xx = CHESS_COORDS[from % 16];
        uint8_t yy = (from / 16) + 1;
        ret += xx;
        ret += intToString(yy);
        //x if capture
        if(capture_type != EMPTY) {
            ret+="x";
            //target piece if not pawn
            if(capture_type != WHITE_PAWN && capture_type != BLACK_PAWN) {
                ret += PIECE_SYMBOLS[capture_type & MASK_PIECE];
            }
        }
        //to
        xx = CHESS_COORDS[to % 16];
        yy = (to / 16) + 1;
        ret += xx;
        ret += intToString(yy);

        if(move_type == MOVETYPE_EN_PASSENT) {
            ret+="ep";
        } else if(move_type == MOVETYPE_PROMOTION_Q) {
            ret+="=Q";
        } else if(move_type == MOVETYPE_PROMOTION_R) {
            ret+="=R";
        } else if(move_type == MOVETYPE_PROMOTION_B) {
            ret+="=B";
        } else if(move_type == MOVETYPE_PROMOTION_N) {
            ret+="=N";
        }
    }

    return ret;
}

#endif // MOVE_HPP
