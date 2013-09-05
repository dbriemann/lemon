#ifndef BITBOARD_HPP
#define BITBOARD_HPP


#include <sstream>
using namespace std;

#include "bb_definitions.hpp"
#include "utility_functions.hpp"
#include "utility_constants.hpp"

/*
 *northwest    north   northeast
  noWe         nort         noEa
          +7    +8    +9
              \  |  /
  west    -1 <-  0 -> +1    east
              /  |  \
          -9    -8    -7
  soWe         sout         soEa
  southwest    south   southeast
 *
 */


struct BitBoard {
    /*
     * BitBoards
     */
    U64 pieces_by_color[2];
    U64 pieces_by_type[6];

    //wrap all following into one U32?
    U8 to_move;
    bool castle_short[2];
    bool castle_long[2];
    Index en_passent_index;
    U8 draw_counter;
    U8 move_number;


    /*
     * Constructors, copy-constructors, destructors
     */
    BitBoard();



    /*
     * Utility methods
     */
    inline void zeroAll();
    inline PieceType get(Index line, Index rank) const;
    inline void set(Index line, Index rank, PieceType type);
    void setFENPosition(string fen);
    string getFENCode() const;
    void setStartingPosition();
    void print() const;


//    int32_t eval();
//    void setStartingPosition();
//    void genPseudoLegalMoves(vector<Move> &moves);
//    void makeMove(Move m);
//    bool undoLastMove();
//    bool makeMoveIfLegal(Move m);
};


BitBoard::BitBoard() {
    zeroAll(); //bitboards
    to_move = WHITE;
    castle_long[WHITE] = castle_long[BLACK] = castle_short[WHITE] = castle_short[BLACK] = true;
    en_passent_index = NO_EN_PASSENT_IDX;
    draw_counter = 0;
    move_number = 1;
}


inline
void BitBoard::zeroAll() {
    pieces_by_color[WHITE] = pieces_by_color[BLACK] = C64(0);
    pieces_by_type[PAWN] = C64(0);
    pieces_by_type[KNIGHT] = C64(0);
    pieces_by_type[BISHOP] = C64(0);
    pieces_by_type[ROOK] = C64(0);
    pieces_by_type[QUEEN] = C64(0);
    pieces_by_type[KING] = C64(0);
}

//inefficient
//used only for utility functionality like extracting the FEN string
inline
PieceType BitBoard::get(Index line, Index rank) const {
    const Index bit = 8*rank + line;
    U8 color_offset = 0;
    PieceType piece = EMPTY;

    if(pieces_by_color[BLACK] & (C64(1) << bit)) {
        color_offset = MASK_COLOR;
    }

    for(int t = 0; t <= KING; t++) {
        if(pieces_by_type[t] & (C64(1) << bit)) {
            piece = t;
            break;
        }
    }

    return piece | color_offset;
}

inline
void BitBoard::set(Index line, Index rank, PieceType type) {
    const U8 bit = 8*rank + line; //8*y + x == bit position in U64

    if(type == EMPTY) {
        pieces_by_color[WHITE] &= ~(C64(1) << bit);
        pieces_by_color[BLACK] &= ~(C64(1) << bit);
        for(int t = 0; t <= KING; t++) {
            pieces_by_type[t] &= ~(C64(1) << bit);
        }
    } else {
        cout << COLORS[(type & MASK_COLOR) >> 3] << " : " << PIECE_SYMBOLS[type] << " at: " << intToString(line) << "," << intToString(rank) << " : "
             << intToString(bit) << endl;
        pieces_by_type[type & MASK_PIECE] |= (C64(1) << bit);
        U8 color = (type & MASK_COLOR) >> 3;
        pieces_by_color[color] |= (C64(1) << bit);
        //pieces_by_color[(color+1) % 2] &= ~(C64(1) << bit);

        cout << "WHITE: " << pieces_by_color[WHITE] << endl;
        cout << "BLACK: " << pieces_by_color[BLACK] << endl;
    }
}

void BitBoard::setStartingPosition() {
    setFENPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void BitBoard::setFENPosition(string fen) {
    Index x = 0;
    Index y = BOARD_SIZE - 1;
    istringstream iss(fen);
    zeroAll();

    //set piece positions
    if(iss) {
       string position;
       iss >> position;
       for(char c : position) {
           if(c == '/') {
               x = 0;
               y--;
           } else if(c >= '1' && c <= '8') {
               int stop = x + (int)(c-'0');
               for(; x < stop; x++) {
                   set(x, y, EMPTY);
               }
           } else {
               //handle pieces
               switch(c) {
                   //black pieces are lowercase letters
                   case 'p':
                       set(x, y, BLACK_PAWN); break;
                   case 'n':
                       set(x, y, BLACK_KNIGHT); break;
                   case 'k':
                       set(x, y, BLACK_KING); break;
                   case 'b':
                       set(x, y, BLACK_BISHOP); break;
                   case 'r':
                       set(x, y, BLACK_ROOK); break;
                   case 'q':
                       set(x, y, BLACK_QUEEN); break;

                   //white pieces are uppercase letters
                   case 'P':
                       set(x, y, WHITE_PAWN); break;
                   case 'N':
                       set(x, y, WHITE_KNIGHT); break;
                   case 'K':
                       set(x, y, WHITE_KING); break;
                   case 'B':
                       set(x, y, WHITE_BISHOP); break;
                   case 'R':
                       set(x, y, WHITE_ROOK); break;
                   case 'Q':
                       set(x, y, WHITE_QUEEN); break;
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
            to_move = WHITE;
        } else if(c == 'b') {
            to_move = BLACK;
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
        castle_long[BLACK] = castle_long[WHITE] = castle_short[BLACK] = castle_short[WHITE] = false;
        string castle;
        iss >> castle;

        for(char c : castle) {
            switch(c) {
                case 'K':
                    castle_short[WHITE]  = true; break;
                case 'Q':
                    castle_long[WHITE] = true; break;
                case 'k':
                    castle_short[BLACK] = true; break;
                case 'q':
                    castle_long[BLACK] = true; break;
                case '-':
                    castle_long[BLACK] = castle_long[WHITE] = castle_short[BLACK] = castle_short[WHITE] = false;
                    break; //disable all
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
            en_passent_index = NO_EN_PASSENT_IDX;
        } else {
            for(int i = 0; i < ep.size(); i+=2) {
                if(ep[i] >= 'a' && ep[i] <= 'h' && ep[i+1] >= '1' && ep[i+1] <= '8') {
                    int x = ep[i] - 'a';
                    int y = ep[i+1] - '1';
                    en_passent_index = (int)(16*y + x);
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
        draw_counter = atoi(s.c_str());
    } else {
        ERROR("FEN CODE CORRUPTED (HALF MOVES UNTIL DRAW)");
    }

    //next move number
    if(iss) {
        string s;
        iss >> s;
        //TODO -- CHECK ILLEGAL VALUES... atoi is bad
        move_number = atoi(s.c_str());
    } else {
        ERROR("FEN CODE CORRUPTED (MOVE NUMBER)");
    }

}

string BitBoard::getFENCode() const {
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
    if(to_move == WHITE) {
        fen += "w";
    } else {
        fen += "b";
    }
    fen += " ";

    //castling rights
    if(castle_short[WHITE]) {
        fen += "K";
    }
    if(castle_long[WHITE]) {
        fen += "Q";
    }
    if(castle_short[BLACK]) {
        fen += "k";
    }
    if(castle_long[BLACK]) {
        fen += "q";
    }
    if(!castle_short[WHITE] && !castle_long[WHITE] && !castle_short[BLACK] && !castle_long[BLACK]) {
        fen += "-";
    }
    fen += " ";

    //en passent
    if(en_passent_index != NO_EN_PASSENT_IDX) {
        char xx = CHESS_COORDS[en_passent_index % 8];
        Index yy = (en_passent_index / 8) + 1;
        fen += xx;
        fen += intToString(yy);
    } else {
        fen += "-";
    }
    fen += " ";

    //50 half moves until draw
    fen += intToString(draw_counter) + " ";

    //moves
    fen += intToString(move_number) + " ";

    return fen;
}

void BitBoard::print() const {
    cout << endl << "******************************************************************************" << endl << endl;
    cout << "      +---+---+---+---+---+---+---+---+" << endl;
    for(int y = BOARD_SIZE-1; y >= 0; y--) {
        cout << "   " << intToString(y+1) << "  |";
        for(int x = 0; x < BOARD_SIZE; x++) {
            char symbol = PIECE_SYMBOLS[get(x,y)];
            if(symbol == ' ') {
                if(x % 2 == 0 && y % 2 == 0) {
                    symbol = '.';
                } else if(x % 2 == 1 && y % 2 == 1) {
                    symbol = '.';
                }
            }
            cout << " " << symbol << " |";
            //cout << " " << (int)get(x,y) << " |";
        }
        cout << endl;
        cout << "      +---+---+---+---+---+---+---+---+" << endl;
    }
    cout << "        a   b   c   d   e   f   g   h" << endl << endl << endl;

    cout << "   Color to move: " << COLORS[to_move] << endl;

    cout << "   Castling rights --- White: ";
    if(castle_short[WHITE]) {
        cout << "0-0 ";
    }
    if(castle_long[WHITE]) {
        cout << "0-0-0 ";
    }
    cout << "   Black: ";
    if(castle_short[BLACK]) {
        cout << "0-0 ";
    }
    if(castle_long[WHITE]) {
        cout << "0-0-0 ";
    }
    cout << endl;

    cout << "   En passent field: ";
    if(en_passent_index != NO_EN_PASSENT_IDX) {
        char xx = CHESS_COORDS[en_passent_index % 8];
        Index yy = (en_passent_index / 8) + 1;
        cout << xx << intToString(yy) << endl;
    } else {
        cout << "-" << endl;
    }

    cout << "   50 moves draw counter: " << intToString(draw_counter) << endl;

    cout << "   Next move number: " << intToString(move_number) << endl;

    cout << "   FEN-Code: " << getFENCode() << endl;
    cout << endl << "******************************************************************************" << endl << endl;
}



#endif // BITBOARD_HPP
