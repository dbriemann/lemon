#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include "Board.hpp"

struct BitBoard : public Board {
    int32_t eval();
    uint8_t get(Index x, Index y) const;
    void set(Index x, Index y, uint8_t value);
    string getFENCode() const;
    void setFENPosition(string fen);
    void setStartingPosition();
    void print() const;
    void genPseudoLegalMoves(vector<Move> &moves);
    void makeMove(Move m);
    bool undoLastMove();
    bool makeMoveIfLegal(Move m);
};

#endif // BITBOARD_HPP
