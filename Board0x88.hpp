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
#include <iostream>
using namespace std;

#include "Board.hpp"
#include "engine_constants.hpp"
#include "utility_constants.hpp"
#include "utility_functions.hpp"

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
    uint8_t get(uint8_t x, uint8_t y);
    void set(uint8_t x, uint8_t y, uint8_t value);
    string getFENCode();
    void setFENCode(string fen);
    void setStartingPosition();
    void print();
};

//constructors & destructor
Board0x88::Board0x88() {
    for(int i = 0; i < 128; i++) {
        board[i] = 0;
    }
    move_number = 1;
    halfmove_number = 0;
    to_move = WHITE;
    en_passent_idx = NO_EN_PASSENT_INDEX;
    white_castle_short = true;
    white_castle_long = true;
    black_castle_short = true;
    black_castle_long = true;
}

//Board interface methods
int32_t Board0x88::eval() {

    return 0;
}

uint8_t Board0x88::get(uint8_t x, uint8_t y) {
    return this->board[16*y+x];
}

void Board0x88::set(uint8_t x, uint8_t y, uint8_t value) {
    this->board[16*y+x] = value;
}

string Board0x88::getFENCode() {
    string fen = "";
    int empty_count = 0;

    for(int y = BOARD_SIZE-1; y >= 0; y--) {
        for(int x = 0; x < BOARD_SIZE; x++) {
            uint8_t c = get(x,y);

            if(c == EMPTY) {
                empty_count++;
            } else {
                if(empty_count > 0) {
                    fen += intToString(empty_count);
                    empty_count = 0;
                }

                switch (c) {
                    case BLACK_PAWN:
                        fen += 'p';
                        break;
                    case BLACK_KNIGHT:
                        fen += 'n';
                        break;
                    case BLACK_BISHOP:
                        fen += 'b';
                        break;
                    case BLACK_ROOK:
                        fen += 'r';
                        break;
                    case BLACK_QUEEN:
                        fen += 'q';
                        break;
                    case BLACK_KING:
                        fen += 'k';
                        break;
                    case WHITE_PAWN:
                        fen += 'P';
                        break;
                    case WHITE_KNIGHT:
                        fen += 'N';
                        break;
                    case WHITE_BISHOP:
                        fen += 'B';
                        break;
                    case WHITE_ROOK:
                        fen += 'R';
                        break;
                    case WHITE_QUEEN:
                        fen += 'Q';
                        break;
                    case WHITE_KING:
                        fen += 'K';
                        break;
                    default:
                        break;
                }
            }
        }

        if(empty_count > 0) {
            fen += intToString(empty_count);
            empty_count = 0;
        }
        if(y > 0) {
            fen += "/";
        }
    }
    fen += " ";

    //to move
    if(this->to_move == WHITE) {
        fen += "w";
    } else {
        fen += "b";
    }
    fen += " ";

    //castling rights
    if(this->white_castle_short) {
        fen += "K";
    }
    if(this->white_castle_long) {
        fen += "Q";
    }
    if(this->black_castle_short) {
        fen += "k";
    }
    if(this->black_castle_long) {
        fen += "q";
    }
    if(!this->white_castle_short && !this->white_castle_long && !this->black_castle_short && !this->black_castle_long) {
        fen += "-";
    }
    fen += " ";

    //en passent
    if(this->en_passent_idx != NO_EN_PASSENT_INDEX) {
        char xx = CHESS_COORDS[this->en_passent_idx % 16];
        uint8_t yy = (this->en_passent_idx / 16) + 1;
        fen += xx;
        fen += intToString(yy);
    } else {
        fen += "-";
    }
    fen += " ";

    //50 half moves until draw
    fen += intToString(this->halfmove_number) + " ";

    //moves
    fen += intToString(this->move_number) + " ";

    return fen;
}

void Board0x88::setFENCode(string fen) {

}

void Board0x88::print() {
    cout << endl << "******************************************************************************" << endl << endl;
    cout << "      +---+---+---+---+---+---+---+---+" << endl;
    for(int y = BOARD_SIZE-1; y >= 0; y--) {
        cout << "   " << intToString(y+1) << "  |";
        for(int x = 0; x < BOARD_SIZE; x++) {
            cout << " " << PIECE_SYMBOLS[this->get(x,y)] << " |";
        }
        cout << endl;
        cout << "      +---+---+---+---+---+---+---+---+" << endl;
    }

    cout << "        a   b   c   d   e   f   g   h" << endl << endl << endl;

    cout << "   Color to move: " << COLORS[this->to_move] << endl;

    cout << "   Castling rights --- White: ";
    if(this->white_castle_short) {
        cout << "0-0 ";
    }
    if(this->white_castle_long) {
        cout << "0-0-0 ";
    }
    cout << "   Black: ";
    if(this->black_castle_short) {
        cout << "0-0 ";
    }
    if(this->black_castle_long) {
        cout << "0-0-0 ";
    }
    cout << endl;

    cout << "   En passent field: ";
    if(this->en_passent_idx != NO_EN_PASSENT_INDEX) {
        char xx = CHESS_COORDS[this->en_passent_idx % 16];
        uint8_t yy = (this->en_passent_idx / 16) + 1;
        cout << xx << intToString(yy) << endl;
    } else {
        cout << "-" << endl;
    }

    cout << "   50 moves draw counter: " << intToString(this->halfmove_number) << endl;

    cout << "   Next move number: " << intToString(this->move_number) << endl;

    cout << "   FEN-Code: " << getFENCode() << endl;
    cout << endl << "******************************************************************************" << endl << endl;
}

void Board0x88::setStartingPosition() {
    setFENCode("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

#endif // BOARD0X88_HPP
