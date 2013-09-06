#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include "lemon.hpp"
#include "Move.hpp"
#include "utility_functions.hpp"

#define MOVELIST_MAX_SIZE 256

struct MoveList {
    U16 size;
    Move moves[MOVELIST_MAX_SIZE] = {0};

    MoveList();

    inline void put(Move m);
    inline void reset();
    //todo get random?
};

MoveList::MoveList() {
    size = 0;
}

inline
void MoveList::put(Move m) {
    if(size+1 >= MOVELIST_MAX_SIZE) {
        ERROR("MoveList::put() : maximum size exceeded -- pls send the position if this happens.");
    } else {
        moves[size] = m;
        size++;
    }
}

inline
void MoveList::reset() {
    size = 0;
}

#endif // MOVELIST_HPP
