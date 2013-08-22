#ifndef BOARD_HPP
#define BOARD_HPP

/*
 * Board is an interface for arbitrary implementations of a chess board
 */

#include <string>
#include <cstdint>
using namespace std;

struct Board {
    virtual int32_t eval() = 0;
    virtual uint8_t get(uint8_t rank, uint8_t file) = 0;
    virtual void set(uint8_t rank, uint8_t file, uint8_t value) = 0;
    virtual string getFENCode() = 0;
    virtual void setFENCode(string fen) = 0;
};

#endif // BOARD_HPP
