#ifndef BITBOARD_HPP
#define BITBOARD_HPP


#include <sstream>
using namespace std;

#include "lemon.hpp"
#include "utility_functions.hpp"
#include "utility_constants.hpp"
#include "Move.hpp"
#include "MoveList.hpp"


struct BitBoard {
    /*
     * BitBoards
     */
    U64 pieces_bb[2];
    U64 pawns_bb;
    U64 knights_bb;
    U64 bishops_bb;
    U64 rooks_bb;
    U64 queens_bb;
    U64 kings_bb;

    //U64 pieces_by_color[2];
    //U64 pieces_by_type[6];

    //wrap all following into one U32?
    U8 player;
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
    U8 get(Index line, Index rank) const;
    void set(Index line, Index rank, OccupancyType type);
    void setFENPosition(string fen);
    string getFENCode() const;
    void setStartingPosition();
    void print() const;

    /*
     * Move generation methods
     */
    inline void genKnightMoves(MoveList &mlist);
    inline U64 genPawnSingleStepTargetMask();

//    int32_t eval();
//    void setStartingPosition();
//    void genPseudoLegalMoves(vector<Move> &moves);
//    void makeMove(Move m);
//    bool undoLastMove();
//    bool makeMoveIfLegal(Move m);
};


BitBoard::BitBoard() {
    zeroAll(); //bitboards
    player = WHITE;
    castle_long[WHITE] = castle_long[BLACK] = castle_short[WHITE] = castle_short[BLACK] = true;
    en_passent_index = NO_EN_PASSENT_IDX;
    draw_counter = 0;
    move_number = 1;
}


inline
void BitBoard::zeroAll() {
    pieces_bb[WHITE] = pieces_bb[BLACK] = C64(0);
    pawns_bb = C64(0);
    knights_bb = C64(0);
    bishops_bb = C64(0);
    rooks_bb = C64(0);
    queens_bb = C64(0);
    kings_bb = C64(0);
}

inline
void BitBoard::genKnightMoves(MoveList &mlist) {
    register U64 bb;
    U64 unocc_bb = ~(pieces_bb[WHITE] | pieces_bb[BLACK]);
    U64 opp_bb = ~pieces_bb[(player + 1) % 2];
    Move m = 0;
    U32 from, to;

    //for every knight of player's color
    U64 play_knights_bb = knights_bb & pieces_bb[player];
    while(play_knights_bb) {
        //extract every knight
        from = bitscanfwd(play_knights_bb);

        //generate non-capture moves
        //gen all targets for current knight
        bb = KNIGHT_TARGET_BBS[from] & unocc_bb;
        //create moves
        while(bb) {
            //extract target square
            to = bitscanfwd(bb);

            m = moveCreate(from, to, KNIGHT, EMPTY, EMPTY, EMPTY, EMPTY, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        //generate captures
        bb = KNIGHT_TARGET_BBS[from] & opp_bb;
        //create captures
        while(bb) {
            //extract target square
            to = bitscanfwd(bb);

            m = moveCreate(from, to, KNIGHT, EMPTY, EMPTY, EMPTY, EMPTY, MOVE_PRE_EVAL_GOODCAP); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        play_knights_bb &= ~iBitMask(from);
    }



}

inline
U64 BitBoard::genPawnSingleStepTargetMask() {
    U64 unocc = ~(pieces_bb[WHITE] | pieces_bb[BLACK]);
    U64 sstmask = 0;

    if(player == WHITE) {
        sstmask = pawns_bb & pieces_bb[WHITE];
        sstmask = _SHIFT_N(sstmask) & unocc;
    } else {
        sstmask = pawns_bb & pieces_bb[BLACK];
        sstmask = _SHIFT_S(sstmask) & unocc;
    }

    return sstmask;
}

//inefficient
//used only for utility functionality like extracting the FEN string
U8 BitBoard::get(Index line, Index rank) const {
    const Index bit = 8*rank + line;
    U8 color_offset = 0;
    U8 piece = EMPTY;

    if(pieces_bb[BLACK] & iBitMask(bit)) {
        color_offset = MASK_COLOR;
    }

    if(pawns_bb & iBitMask(bit)) {
        piece = WPAWN;
    } else if(knights_bb & iBitMask(bit)) {
        piece = WKNIGHT;
    } else if(bishops_bb & iBitMask(bit)) {
        piece = WBISHOP;
    } else if(rooks_bb & iBitMask(bit)) {
        piece = WROOK;
    } else if(queens_bb & iBitMask(bit)) {
        piece = WQUEEN;
    } else if(kings_bb & iBitMask(bit)) {
        piece = WKING;
    }

    return piece | color_offset;
}

inline
void BitBoard::set(Index line, Index rank, OccupancyType type) {
    const U8 bit = 8*rank + line; //8*y + x == bit position in U64

    if(type == EMPTY) {
        U64 rmask = ~(iBitMask(bit));
        pieces_bb[WHITE] &= rmask;
        pieces_bb[BLACK] &= rmask;
        pawns_bb &= rmask;
        knights_bb &= rmask;
        bishops_bb &= rmask;
        rooks_bb &= rmask;
        queens_bb &= rmask;
        kings_bb &= rmask;
    } else {
        U64 amask = iBitMask(bit);
        switch(type & MASK_PIECE) {
            case PAWN:
                pawns_bb |= amask; break;
            case KNIGHT:
                knights_bb |= amask; break;
            case BISHOP:
                bishops_bb |= amask; break;
            case ROOK:
                rooks_bb |= amask; break;
            case QUEEN:
                queens_bb |= amask; break;
            case KING:
                kings_bb |= amask; break;
            default:
                break;
        }
        U8 color = (type & MASK_COLOR) >> 3;
        pieces_bb[color] |= (C64(1) << bit);
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
                       set(x, y, BPAWN); break;
                   case 'n':
                       set(x, y, BKNIGHT); break;
                   case 'k':
                       set(x, y, BKING); break;
                   case 'b':
                       set(x, y, BBISHOP); break;
                   case 'r':
                       set(x, y, BROOK); break;
                   case 'q':
                       set(x, y, BQUEEN); break;

                   //white pieces are uppercase letters
                   case 'P':
                       set(x, y, WPAWN); break;
                   case 'N':
                       set(x, y, WKNIGHT); break;
                   case 'K':
                       set(x, y, WKING); break;
                   case 'B':
                       set(x, y, WBISHOP); break;
                   case 'R':
                       set(x, y, WROOK); break;
                   case 'Q':
                       set(x, y, WQUEEN); break;
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
            player = WHITE;
        } else if(c == 'b') {
            player = BLACK;
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
                    case BPAWN:
                        fen += 'p'; break;
                    case BKNIGHT:
                        fen += 'n'; break;
                    case BBISHOP:
                        fen += 'b'; break;
                    case BROOK:
                        fen += 'r'; break;
                    case BQUEEN:
                        fen += 'q'; break;
                    case BKING:
                        fen += 'k'; break;
                    case WPAWN:
                        fen += 'P'; break;
                    case WKNIGHT:
                        fen += 'N'; break;
                    case WBISHOP:
                        fen += 'B'; break;
                    case WROOK:
                        fen += 'R'; break;
                    case WQUEEN:
                        fen += 'Q'; break;
                    case WKING:
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
    if(player == WHITE) {
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

    cout << "   Color to move: " << COLORS[player] << endl;

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
