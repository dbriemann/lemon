#ifndef BOARD_HPP
#define BOARD_HPP

/*
 * Board is an interface for arbitrary implementations of a chess board
 */

#include <string>
#include <cstdint>
#include <vector>
using namespace std;

#include "board0x88_constants.hpp"
#include "Move.hpp"

struct Board {
    virtual int32_t eval() = 0;
    virtual uint8_t get(Index x, Index y) const = 0;
    virtual void set(Index x, Index y, uint8_t value) = 0;
    virtual string getFENCode() const = 0;
    virtual void setFENPosition(string fen) = 0;
    virtual void setStartingPosition() = 0;
    virtual void print() const = 0;
    virtual vector<Move> genPseudoLegalMoves() = 0;
    virtual void makeMove(const Move m) = 0;
};

#endif // BOARD_HPP
