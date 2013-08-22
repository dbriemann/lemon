#ifndef BOARD0X88_HPP
#define BOARD0X88_HPP

/*
 * Board0x88 is a chess board implementation
 *
 * it uses a 1d array as the following representation
 *
    112 113 114 115 116 117 118 119 | 120 121 122 123 124 125 126 127
     96  97  98  99 100 101 102 103 | 104 105 106 107 108 109 110 111
     80  81  82  83  84  85  86  87 |  88  89  90  91  92  93  94  95
     64  65  66  67  68  69  70  71 |  72  73  74  75  76  77  78  79
     48  49  50  51  52  53  54  55 |  56  57  58  59  60  61  62  63
     32  33  34  35  36  37  38  39 |  40  41  42  43  44  45  46  47
     16  17  18  19  20  21  22  23 |  24  25  26  27  28  29  30  31
      0   1   2   3   4   5   6   7 |   8   9  10  11  12  13  14  15
 *
 *
 */

#include <cstdint>
using namespace std;

#include "Board.hpp"

struct Board0x88 : public Board {
    uint8_t board[128];
    uint16_t move_number;
    uint16_t halfmove_number;
    uint8_t to_move;
    uint8_t en_passent_idx;
    bool white_castle_short;
    bool white_castle_long;
    bool black_castle_short;
    bool black_castle_long;

    //constructors & destructor
    Board0x88();

    //Board interface methods
    int32_t eval();
    uint8_t get(uint8_t rank, uint8_t file);
    void set(uint8_t rank, uint8_t file, uint8_t value);
    string getFENCode();
    void setFENCode(string fen);
};

//constructors & destructor
Board0x88::Board0x88() {

}

//Board interface methods
int32_t Board0x88::eval() {

    return 0;
}

uint8_t Board0x88::get(uint8_t rank, uint8_t file) {

    return 0;
}

void Board0x88::set(uint8_t rank, uint8_t file, uint8_t value) {

}

string Board0x88::getFENCode() {

    return "";
}

void Board0x88::setFENCode(string fen) {

}


#endif // BOARD0X88_HPP
