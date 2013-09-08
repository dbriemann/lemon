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
    U64 pieces_by_color_bb[2];
    U64 pieces_by_type_bb[6];

    /*
     * Arrays to help in certain situation
     */
    U8 occupancy[64];
    //U8 kings[2];

    //wrap all following into one U32?
    U8 player;
    bool castle_short[2];
    bool castle_long[2];
    Square en_passent_sq;
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
    U8 get(Square line, Square rank) const;
    void set(Square line, Square rank, OccupancyType type);
    void setFENPosition(string fen);
    string getFENCode() const;
    void setStartingPosition();
    void print() const;

    /*
     * Move generation methods
     */
    inline void genKnightMoves(MoveList &mlist);
    inline void genPawnMoves(MoveList &mlist);

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
    en_passent_sq = NONE;
    draw_counter = 0;
    move_number = 1;
}


inline
void BitBoard::zeroAll() {
    pieces_by_color_bb[WHITE] = pieces_by_color_bb[BLACK] = C64(0);
    for(int i = PAWN; i <= KING; i++) {
        pieces_by_type_bb[i] = C64(0);
    }
    for(int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
        occupancy[i] = EMPTY;
    }
}

inline
void BitBoard::genKnightMoves(MoveList &mlist) {
    register U64 bb;
    const U64 unocc_bb = ~(pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK]);
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    Move m = 0;
    U32 from, to;

    //for every knight of player's color
    U64 play_knights_bb = pieces_by_type_bb[KNIGHT] & pieces_by_color_bb[player];
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

            m = moveCreate(from, to, KNIGHT, CAPTURE_NO, EMPTY, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        //generate captures
        bb = KNIGHT_TARGET_BBS[from] & opp_bb;
        //create captures
        while(bb) {
            //extract target square
            to = bitscanfwd(bb);
            //TODO check capture goodness

            m = moveCreate(from, to, KNIGHT, CAPTURE_YES, EMPTY, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_GOODCAP); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        play_knights_bb &= ~iBitMask(from);
    }



}

inline
void BitBoard::genPawnMoves(MoveList &mlist) {
    const U64 unocc_bb = ~(pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK]);
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    const U64 own_bb = pieces_by_color_bb[player];
    Move m = 0;
    U32 from, to;
    register U64 bb;
    U64 two_steppers, capture_west_targets, capture_east_targets, ep_takers, promo_rank;
    U64 ep_target = iBitMask(en_passent_sq) & EP_RANKS;

    //TODO remove if white/black conditionals somehow
    //TODO promotions..efficient?

    //one step advances
    bb = pieces_by_type_bb[PAWN] & pieces_by_color_bb[player];   

    if(player == WHITE) {
        promo_rank = RANK_8;
        ep_takers = (_SHIFT_SE(ep_target) & (own_bb & ~FILE_A)) | (_SHIFT_SW(ep_target) & (own_bb & ~FILE_H));
        capture_east_targets = _SHIFT_NE(bb) & (opp_bb & ~FILE_A);
        capture_west_targets = _SHIFT_NW(bb) & (opp_bb & ~FILE_H);
        bb = _SHIFT_N(bb) & unocc_bb;
        two_steppers = _SHIFT_N(bb & RANK_3) & unocc_bb; //only white pawns that are on rank 3 AFTER one step could a do a double step
    } else {
        promo_rank = RANK_1;
        ep_takers = (_SHIFT_NE(ep_target) & (own_bb & ~FILE_A)) | (_SHIFT_NW(ep_target) & (own_bb & ~FILE_H));
        capture_east_targets = _SHIFT_SE(bb) & (opp_bb & ~FILE_A);
        capture_west_targets = _SHIFT_SW(bb) & (opp_bb & ~FILE_H);
        bb = _SHIFT_S(bb) & unocc_bb;
        two_steppers = _SHIFT_S(bb & RANK_6) & unocc_bb; //only black pawns that are on rank 6 AFTER one step could a do a double step
    }

    //extract single step moves
    while(bb) {
        to = bitscanfwd(bb);
        from = (int)to - PAWN_MOVE_DIRECTIONS[player];

        //check promotion
        if(iBitMask(to) & promo_rank) {
            m = moveCreate(from, to, PAWN, CAPTURE_NO, QUEEN, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_NO, ROOK, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_NO, BISHOP, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_NO, KNIGHT, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
        } else {
            m = moveCreate(from, to, PAWN, CAPTURE_NO, EMPTY, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_DEFAULT);
            mlist.put(m);
        }

        bb &= ~iBitMask(to);
    }

    //extract double step moves
    bb = two_steppers;
    while(bb) {
        //TODO creates en passent square..?!
        to = bitscanfwd(bb);
        from = (int)to - 2*PAWN_MOVE_DIRECTIONS[player];

        m = moveCreate(from, to, PAWN, CAPTURE_NO, EMPTY, EP_TYPE_CREATE, NONE, MOVE_PRE_EVAL_DEFAULT);
        mlist.put(m);

        bb &= ~iBitMask(to);
    }

    //extract captures
    //east
    bb = capture_east_targets;
    while(bb) {
        to = bitscanfwd(bb);
        from = (int)to - PAWN_CAP_EAST_DIRECTIONS[player];
        //TODO check capture goodness

        //check promotion
        if(iBitMask(to) & promo_rank) {
            m = moveCreate(from, to, PAWN, CAPTURE_YES, QUEEN, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, ROOK, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, BISHOP, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, KNIGHT, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
        } else {
            m = moveCreate(from, to, PAWN, CAPTURE_YES, EMPTY, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_GOODCAP);
            mlist.put(m);
        }

        bb &= ~iBitMask(to);
    }

    //west
    bb = capture_west_targets;
    while(bb) {
        to = bitscanfwd(bb);
        from = (int)to - PAWN_CAP_WEST_DIRECTIONS[player];
        //TODO check capture goodness

        //check promotion
        if(iBitMask(to) & promo_rank) {
            m = moveCreate(from, to, PAWN, CAPTURE_YES, QUEEN, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, ROOK, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, BISHOP, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, KNIGHT, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
        } else {
            m = moveCreate(from, to, PAWN, CAPTURE_YES, EMPTY, EP_TYPE_NONE, NONE, MOVE_PRE_EVAL_GOODCAP);
            mlist.put(m);
        }

        bb &= ~iBitMask(to);
    }

    //if there is a current ep. square check if pawns can capture there
    if(en_passent_sq) {
        bb = ep_takers;

        while(bb) {
            from = bitscanfwd(bb);

            m = moveCreate(from, en_passent_sq, PAWN, CAPTURE_YES, EMPTY, EP_TYPE_CAPTURE, NONE, MOVE_PRE_EVAL_GOODCAP);
            mlist.put(m);

            bb &= ~iBitMask(from);
        }
    }
}

//inefficient
//used only for utility functionality like extracting the FEN string
U8 BitBoard::get(Square line, Square rank) const {
    const Square bit = SQ(line,rank);//8*rank + line;
    U8 color_offset = 0;
    U8 piece = EMPTY;

    if(pieces_by_color_bb[BLACK] & iBitMask(bit)) {
        color_offset = MASK_COLOR;
    }


    //TODO
    //piece = occupancy[bit];
    for(int p = PAWN; p <= KING; p++) {
        if(pieces_by_type_bb[p] & iBitMask(bit)) {
            piece = p;
            break;
        }
    }

    if(piece != occupancy[bit]) {
        ERROR("BitBoard::get() : boards not synced...");
    }

    return piece | color_offset;
}

inline
void BitBoard::set(Square line, Square rank, OccupancyType type) {
    const U8 bit = 8*rank + line; //8*y + x == bit position in U64

    if(type == EMPTY) {
        occupancy[bit] = EMPTY;
        U64 rmask = ~(iBitMask(bit));
        pieces_by_color_bb[WHITE] &= rmask;
        pieces_by_color_bb[BLACK] &= rmask;
        for(int p = PAWN; p <= KING; p++) {
            pieces_by_type_bb[p] &= rmask;
        }
    } else {
        occupancy[bit] = (type & MASK_PIECE);
        U64 amask = iBitMask(bit);
        pieces_by_type_bb[type & MASK_PIECE] |= amask;
        U8 color = (type & MASK_COLOR) >> 3;
        pieces_by_color_bb[color] |= iBitMask(bit);
    }
}

void BitBoard::setStartingPosition() {
    setFENPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void BitBoard::setFENPosition(string fen) {
    Square x = 0;
    Square y = BOARD_SIZE - 1;
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
            en_passent_sq = NONE;
        } else {
            for(int i = 0; i < ep.size(); i+=2) {
                if(ep[i] >= 'a' && ep[i] <= 'h' && ep[i+1] >= '1' && ep[i+1] <= '8') {
                    int x = ep[i] - 'a';
                    int y = ep[i+1] - '1';
                    en_passent_sq = SQ(x,y);
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
    if(en_passent_sq != NONE) {
        char xx = CHESS_COORDS[FILE(en_passent_sq)];
        Square yy = RANK(en_passent_sq) + 1;
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
    if(en_passent_sq != NONE) {
        char xx = CHESS_COORDS[FILE(en_passent_sq)];
        Square yy = RANK(en_passent_sq) + 1;
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
