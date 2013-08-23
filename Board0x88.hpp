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
#include <sstream>
#include <cstdlib>
#include <list>
using namespace std;

#include "engine_constants.hpp"
#include "utility_constants.hpp"
#include "utility_functions.hpp"
#include "board0x88_constants.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Move.hpp"


struct Board0x88 : public Board {
    list<Piece> white_pieces;
    list<Piece> black_pieces;
    Piece * squares[128];
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
    uint8_t get(Index x, Index y) const;
    void set(Index x, Index y, uint8_t value);
    string getFENCode() const;
    void setFENPosition(string fen);
    void setStartingPosition();
    void print() const;
    void genPseudoLegalMoves();

    //non-interface functions
    void genPseudoLegalMovesForPieces(list<Piece> &pieces);
    void genPseudoLegalKingMoves(Index square);
};

//constructors & destructor
Board0x88::Board0x88() {
    white_pieces = list<Piece>();
    black_pieces = list<Piece>();

    for(int i = 0; i < 128; i++) {
        squares[i] = NULL;
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

uint8_t Board0x88::get(Index x, Index y) const {
    const Piece * p = squares[16*y+x];
    if(p != NULL) {
        return p->type;
    } else {
        return EMPTY;
    }
}

void Board0x88::set(Index x, Index y, uint8_t value) {
    //cout << "SET: " << (int)x << "," << (int)y << " : " << PIECE_SYMBOLS[value] << endl;
    const Index square = 16*y+x;
    Piece * p = NULL;
    if(value >= WHITE_PAWN && value <= WHITE_QUEEN) {
        //cout << "ADD WHITE " << PIECE_SYMBOLS[value] << endl;
        white_pieces.push_back(Piece(value, square));
        p = &white_pieces.back();
    } else if(value >= BLACK_PAWN && value <= BLACK_QUEEN) {
        //cout << "ADD BLACK " << PIECE_SYMBOLS[value] << endl;
        black_pieces.push_back(Piece(value, square));
        p = &black_pieces.back();
    }
    squares[square] = p;
}

void Board0x88::genPseudoLegalMoves() {
    if(to_move == WHITE) {
        genPseudoLegalMovesForPieces(white_pieces);
    } else {
        genPseudoLegalMovesForPieces(black_pieces);
    }
}

void Board0x88::genPseudoLegalMovesForPieces(list<Piece> &pieces) {
    //for every piece
    for(Piece &p : pieces) {
        uint8_t raw_type = p.type & MASK_RAW_PIECE_TYPE;

        //test piece
        switch(raw_type) {
            case PAWN:
                break;
            case KNIGHT:
                break;
            case BISHOP:
                break;
            case ROOK:
                break;
            case QUEEN:
                break;
            case KING:
                genPseudoLegalKingMoves(p.square);
                break;
        }
    }
}

void Board0x88::genPseudoLegalKingMoves(Index square) {
    Index target;
    Index offset = 0;
    if(to_move == BLACK) {
        offset = 0x08;
    }

    for(const int8_t &d : KING_DELTAS) {
        target = static_cast<int8_t>(square) + d;
        //cout << "TARGET: " << (int)target << endl;

        //if target is not occupied by a piece of the same color
        if(target >= 0) {
            Move m = 0;
            uint8_t mtype = MOVETYPE_ORDINARY;
            uint8_t capture = EMPTY;

            if(squares[target] == NULL) {
                //move
                moveMake(m, WHITE_KING + offset, square, target, mtype, capture);
                cout << moveToString(m) << ", " << endl;
                //TODO.. castling
            } else if(((squares[target]->type & MASK_COLOR) >> 3) != to_move) {
                //capture
                capture = EMPTY; //TODO
                moveMake(m, WHITE_KING + offset, square, target, mtype, capture);
                cout << moveToString(m) << ", " << endl;
            }
        }
    }
}

string Board0x88::getFENCode() const {
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
                        fen += 'p'; break;
                    case BLACK_KNIGHT:
                        fen += 'n'; break;
                    case BLACK_BISHOP:
                        fen += 'b'; break;
                    case BLACK_ROOK:
                        fen += 'r'; break;
                    case BLACK_QUEEN:
                        fen += 'q'; break;
                    case BLACK_KING:
                        fen += 'k'; break;
                    case WHITE_PAWN:
                        fen += 'P'; break;
                    case WHITE_KNIGHT:
                        fen += 'N'; break;
                    case WHITE_BISHOP:
                        fen += 'B'; break;
                    case WHITE_ROOK:
                        fen += 'R'; break;
                    case WHITE_QUEEN:
                        fen += 'Q'; break;
                    case WHITE_KING:
                        fen += 'K'; break;
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
        Index yy = (this->en_passent_idx / 16) + 1;
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

void Board0x88::setFENPosition(string fen) {
    Index x = 0;
    Index y = BOARD_SIZE - 1;
    istringstream iss(fen);
    white_pieces.clear();
    black_pieces.clear();

    //set piece positions
    if(iss) {
       string position;
       iss >> position;
       for(char c : position) {
           if(c == '/') {
               x = 0;
               y--;
           } else if(c >= '1' && c <= '8') {
               int stop = x + static_cast<int>(c-'0');
               for(; x < stop; x++) {
                   this->set(x, y, EMPTY);
               }
           } else {
               //handle pieces
               switch(c) {
                   //black pieces are lowercase letters
                   case 'p':
                       this->set(x, y, BLACK_PAWN); break;
                   case 'n':
                       this->set(x, y, BLACK_KNIGHT); break;
                   case 'k':
                       this->set(x, y, BLACK_KING); break;
                   case 'b':
                       this->set(x, y, BLACK_BISHOP); break;
                   case 'r':
                       this->set(x, y, BLACK_ROOK); break;
                   case 'q':
                       this->set(x, y, BLACK_QUEEN); break;

                   //white pieces are uppercase letters
                   case 'P':
                       this->set(x, y, WHITE_PAWN); break;
                   case 'N':
                       this->set(x, y, WHITE_KNIGHT); break;
                   case 'K':
                       this->set(x, y, WHITE_KING); break;
                   case 'B':
                       this->set(x, y, WHITE_BISHOP); break;
                   case 'R':
                       this->set(x, y, WHITE_ROOK); break;
                   case 'Q':
                       this->set(x, y, WHITE_QUEEN); break;
                   default:
                       string err = "FEN CODE CORRUPTED (PIECE -> ";
                       err += c; err += ")";
                       ERROR(err); break;
               }

               x++;
           }
       }
    } else {
        ERROR("FEN CODE CORRUPTED (POSITION)");
    }

    //set player to move
    if(iss) {
        char c;
        iss >> c;

        if(c == 'w') {
            this->to_move = WHITE;
        } else if(c == 'b') {
            this->to_move = BLACK;
        } else {
            string err = "FEN CODE CORRUPTED (COLOR ->";
            err += c; err += ")";
            ERROR(err);
        }
    } else {
        ERROR("FEN CODE CORRUPTED (PLAYER TO MOVE)");
    }

    //set castling rights
    if(iss) {
        this->black_castle_long = this->black_castle_short = this->white_castle_long = this->white_castle_short = false;
        string castle;
        iss >> castle;

        for(char c : castle) {
            switch(c) {
                case 'K':
                    this->white_castle_short = true; break;
                case 'Q':
                    this->white_castle_long = true; break;
                case 'k':
                    this->black_castle_short = true; break;
                case 'q':
                    this->black_castle_long = true; break;
                default:
                    string err = "FEN CODE CORRUPTED (CASTLING RIGHTS ->";
                    err += c; err += ")";
                    ERROR(err);
                    break;
            }
        }
    } else {
        ERROR("FEN CODE CORRUPTED (CASTLING RIGHTS)");
    }

    //set en passent state
    if(iss) {
        string ep;
        iss >> ep;

        if(ep == "-") {
            //no en passent
            this->en_passent_idx = NO_EN_PASSENT_INDEX;
        } else {
            for(int i = 0; i < ep.size(); i+=2) {
                if(ep[i] >= 'a' && ep[i] <= 'h' && ep[i+1] >= '1' && ep[i+1] <= '8') {
                    int x = ep[i] - 'a';
                    int y = ep[i+1] - '1';
                    this->en_passent_idx = static_cast<int>(16*y + x);
                } else {
                    string err = "FEN CODE CORRUPTED (EN PASSENT ->";
                    err += ep; err += ")";
                    ERROR(err);
                }
            }
            if(ep.size() > 4 && ep[0] >= 'a' && ep[0] <= 'h' && ep[1] >= '1' && ep[2] <= '8') {

            } else {

            }
        }

    } else {
        ERROR("FEN CODE CORRUPTED (EN PASSENT)");
    }

    //half moves until draw
    if(iss) {
        string s;
        iss >> s;
        //TODO -- CHECK ILLEGAL VALUES... atoi is bad
        this->halfmove_number = atoi(s.c_str());
    } else {
        ERROR("FEN CODE CORRUPTED (HALF MOVES UNTIL DRAW)");
    }

    //next move number
    if(iss) {
        string s;
        iss >> s;
        //TODO -- CHECK ILLEGAL VALUES... atoi is bad
        this->move_number = atoi(s.c_str());
    } else {
        ERROR("FEN CODE CORRUPTED (MOVE NUMBER)");
    }

}

void Board0x88::print() const {
    cout << endl << "******************************************************************************" << endl << endl;
    cout << "      +---+---+---+---+---+---+---+---+" << endl;
    for(int y = BOARD_SIZE-1; y >= 0; y--) {
        cout << "   " << intToString(y+1) << "  |";
        for(int x = 0; x < BOARD_SIZE; x++) {
            cout << " " << PIECE_SYMBOLS[this->get(x,y)] << " |";
            //cout << " " << (int)this->get(x,y) << " |";
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
        Index yy = (this->en_passent_idx / 16) + 1;
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
    setFENPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

#endif // BOARD0X88_HPP
