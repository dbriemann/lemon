#ifndef PIECE_HPP
#define PIECE_HPP

#include <cstdint>
using namespace std;

#include "engine_constants.hpp"
#include "board0x88_constants.hpp"

struct Piece {
    uint8_t type;
    uint8_t square;

    Piece();
    Piece(uint8_t ty, uint8_t sq);
};

Piece::Piece() {
    type = EMPTY;
    square = 0x08; //out of board
}

Piece::Piece(uint8_t ty, uint8_t sq) {
    type = ty;
    square = sq;
}

#endif // PIECE_HPP
