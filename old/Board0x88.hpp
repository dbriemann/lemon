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
#include <vector>
#include <stack>
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
    list<Piece> pieces[2];
    Piece * squares[128];
    uint16_t move_number; //TODO uint8_t enough.. max 255 moves
    //uint8_t halfmove_draw_counter;
    uint8_t to_move;
    uint8_t en_passent_square;
    bool castle_short[2];
    bool castle_long[2];
    Index king_square[2];
    stack<Move> move_history;
    stack<uint8_t> draw_count_history; //TODO -- integrate into Move (64 bit)

    //constructors & destructor
    Board0x88();
    ~Board0x88();

    //Board interface methods
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

    //non-interface functions
    void genPseudoLegalMovesForPieces(list<Piece> &pieces, vector<Move> &moves);
    void genPseudoLegalKingMoves(const Index square, vector<Move> &moves);
    void genPseudoLegalPawnMoves(const Index square, vector<Move> &moves);
    void genPseudoLegalKnightMoves(const Index square, vector<Move> &moves);
    void genPseudoLegalSlidingPieceMoves(const Index square, const uint8_t ptype, vector<Move> &moves, const Offset *slider_deltas, const int deltas_size);
    bool isOnBoard(const Index idx) const;
    bool removePiece(const Index piece_square, list<Piece> &pieces);
    void addPiece(const uint8_t ptype, const Index piece_square, list<Piece> &pieces);
};

//constructors & destructor
Board0x88::Board0x88() {
    pieces[WHITE] = list<Piece>();
    pieces[BLACK] = list<Piece>();

    for(int i = 0; i < 128; i++) {
        squares[i] = NULL;
    }
    move_number = 1;
    move_history = stack<Move>();
    draw_count_history = stack<uint8_t>();
    //halfmove_draw_counter = 0;
    to_move = WHITE;
    en_passent_square = NO_EN_PASSENT;
    king_square[WHITE] = 0;
    king_square[BLACK] = 0;

    castle_short[WHITE] = true;
    castle_short[BLACK] = true;
    castle_long[WHITE] = true;
    castle_long[BLACK] = true;
}

Board0x88::~Board0x88() {
    for(int i = 0; i < 128; i++) {
        squares[i] = NULL;
    }

    pieces[WHITE].clear();
    pieces[BLACK].clear();
}

bool Board0x88::isOnBoard(const Index idx) const {
    //return (idx >= 0 && !(idx & 0x88)); //TODO.. 0x88 trick with subtraction(uint) ok?
    return !(idx & 0x88);
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

//TODO inefficient
void Board0x88::set(Index x, Index y, uint8_t value) {
    const Index square = 16*y+x;
    Piece * p = NULL;
    if(value >= WHITE_PAWN && value <= WHITE_QUEEN) {
        if(value == WHITE_KING) {
            king_square[WHITE] = square;
        }
        pieces[WHITE].push_back(Piece(value, square));
        p = &pieces[WHITE].back();
    } else if(value >= BLACK_PAWN && value <= BLACK_QUEEN) {
        if(value == BLACK_KING) {
            king_square[BLACK] = square;
        }
        pieces[BLACK].push_back(Piece(value, square));
        p = &pieces[BLACK].back();
    }
    squares[square] = p;
}

void Board0x88::addPiece(const uint8_t ptype, const Index piece_square, list<Piece> &pieces) {
    pieces.push_back(Piece(ptype, piece_square));
    squares[piece_square] = &pieces.back();
}

bool Board0x88::removePiece(const Index piece_square, list<Piece> &pieces) {
    auto iter = pieces.begin();

    while(iter != pieces.end()) {
        if(iter->square == piece_square) {
            break;
        }
        iter++;
    }

    if(iter == pieces.end()) {        
        this->print();
        ERROR("Board0x88::removePiece(piece_square=" + intToString(piece_square) + ")");
        for(Piece &p : pieces) {
            char xx = CHESS_COORDS[p.square % 16];
            Index yy = (p.square / 16) + 1;
            cout << PIECE_SYMBOLS[p.type] << ": " << xx << (int)yy << " # ";
        }
        cout << endl;
        return false;
    }

    pieces.erase(iter);
    squares[piece_square] = NULL;
    return true;
}

bool Board0x88::undoLastMove() {
    if(move_history.empty()) {
        //ERROR("Board0x88::undoLastMove(): cannot undo/first move.");
        return false; //first move..
    }
    //take last move from stack
    Move last_move = move_history.top();
    move_history.pop();

    //reverse from and to -> move is undone
    swap(last_move.from, last_move.to);

    //unmake every en passent information
    en_passent_square = NO_EN_PASSENT;

    //unmake halfmove draw counter
    draw_count_history.pop();

    /*
    if(squares[last_move.from] == NULL) {
        cout << last_move.toString() << endl;
        print();
    }*/
    //move piece backwards
    swap(squares[last_move.from], squares[last_move.to]);
    squares[last_move.to]->square = last_move.to;


    //if move is a promotion.. revert to pawn
    if(last_move.move_type == MOVETYPE_PROMOTION_B || last_move.move_type == MOVETYPE_PROMOTION_N
      || last_move.move_type == MOVETYPE_PROMOTION_R || last_move.move_type == MOVETYPE_PROMOTION_Q) {
        squares[last_move.to]->type = PAWN + COLOR_PIECE_OFFSET[(to_move+1) % 2];
    }

    //if a capture occured, put back piece
    if(last_move.capture_type != EMPTY) {
        if(last_move.move_type == MOVETYPE_EN_PASSENT) {
            //en passent capture
            addPiece(last_move.capture_type, last_move.from + PAWN_DELTAS[to_move][0], pieces[to_move]);
        } else {
            //default capture
            addPiece(last_move.capture_type, last_move.from, pieces[to_move]);
        }
    }

    //first move?
    if(!move_history.empty()) {
        //else check previous move for en passent
        Move llmove = move_history.top(); //don't pop move

        //if previous move created en passent opportunity..
        if((llmove.piece_type & MASK_PIECE) == PAWN) {
            //if pawn move was a two step move..
            uint8_t dist = abs((int)llmove.from - (int)llmove.to);

            if(dist == PAWN_DOUBLE_STEP_DIST) {
                en_passent_square = llmove.from + PAWN_DELTAS[to_move][0];
            }
        }
    }

    //revert opp castling rights
    if(last_move.castling_mask & DISABLE_SHORT_OPP_CASTLING) {
        castle_short[to_move] = true;
    }
    if(last_move.castling_mask & DISABLE_LONG_OPP_CASTLING) {
        castle_long[to_move] = true;
    }

    //change color
    to_move = (to_move + 1) % 2;

    if((last_move.piece_type & MASK_PIECE) == KING) {
        king_square[to_move] = last_move.to;
    }

    //revert castling rights
    if(last_move.castling_mask & DISABLE_SHORT_CASTLING) {
        castle_short[to_move] = true;
    }
    if(last_move.castling_mask & DISABLE_LONG_CASTLING) {
        castle_long[to_move] = true;
    }

    //special moves
    if(last_move.move_type == MOVETYPE_CASTLE_LONG) {
        swap(squares[CASTLE_LONG_ROOK_PATH[to_move][1]], squares[CASTLE_LONG_ROOK_PATH[to_move][0]]); //move rook back
        squares[CASTLE_LONG_ROOK_PATH[to_move][0]]->square = CASTLE_LONG_ROOK_PATH[to_move][0];
    } else if(last_move.move_type == MOVETYPE_CASTLE_SHORT) {
        swap(squares[CASTLE_SHORT_ROOK_PATH[to_move][1]], squares[CASTLE_SHORT_ROOK_PATH[to_move][0]]); //move rook back
        squares[CASTLE_SHORT_ROOK_PATH[to_move][0]]->square = CASTLE_SHORT_ROOK_PATH[to_move][0];
    }

    if(to_move == BLACK) {
        move_number--;
    }

    return true;
}

//executes a move, tests the resulting position for legality, and undos the move if it was illegal
//legal: return true
//illegal: return false
bool Board0x88::makeMoveIfLegal(Move m) {
    //create vector with squares to be checked
    vector<Index> check_squares;

    //squares passed by castling
    if(m.move_type == MOVETYPE_CASTLE_SHORT) {
        check_squares.push_back(king_square[to_move]); //position of king
        check_squares.push_back(CASTLE_SHORT_PATH[to_move][0]);
        check_squares.push_back(CASTLE_SHORT_PATH[to_move][1]);
    } else if(m.move_type == MOVETYPE_CASTLE_LONG) {
        check_squares.push_back(king_square[to_move]); //position of king
        check_squares.push_back(CASTLE_LONG_PATH[to_move][0]);
        check_squares.push_back(CASTLE_LONG_PATH[to_move][1]);
    }

    //execute move
    makeMove(m); //changes color/to_move

    uint8_t attacked_color = (to_move + 1) % 2;
    check_squares.push_back(king_square[attacked_color]); //position of king


    uint8_t ptype;
    Index wander_index;

    //test legality of position
    for(Piece &p : pieces[to_move]) {
        ptype = p.type & MASK_PIECE;
        if(ptype == PAWN) { //pawns must have color info
            ptype = p.type;
        }

        //formula: attacked_square - attacking_square + 128 = pieces able to attack
        for(Index &i : check_squares) {
            uint8_t atk_index = 128 + i - p.square;
            //check if the piece can attack the check square
            if(ATTACK_ARRAY[atk_index] & PIECE_ATTACK_MASK[ptype]) {
                //attack is possible..
                wander_index = p.square + DELTA_ARRAY[atk_index];
                //now calculate if deltas lead to target square without hinderance
                while(wander_index != i) {
                    if(squares[wander_index] != NULL) {
                        break;
                    }
                    wander_index += DELTA_ARRAY[atk_index];
                }
                if(wander_index == i) {
                    /*
                    ERROR("KING SQUARE " + intToString(king_square[attacked_color]));
                    ERROR("ATTACKED SQUARE " + intToString(i));
                    ERROR("HIT TARGET -> ILLEGAL MOVE");
                    ERROR("EVIL SQUARE: " + intToString(p.square));
                    ERROR(m.toString());
                    this->print();
                    */
                    undoLastMove();
                    return false;
                }
            }
        }
    }

    return true; //if we reach this the move was legal
}

void Board0x88::makeMove(Move m) {
    //after swap piece is removed at origin, only used if capture occurs, en passent overwrites this
    Index capture_square = m.from;
    en_passent_square = NO_EN_PASSENT;

    //move piece
    //squares[from]->square = to;
    swap(squares[m.from], squares[m.to]);
    squares[m.to]->square = m.to;

    switch(m.move_type) {
        case MOVETYPE_CASTLE_LONG:
            /*
            if(squares[CASTLE_LONG_ROOK_PATH[to_move][0]] == NULL) {
                swap(squares[m.from], squares[m.to]);
                squares[m.from]->square = m.from;
                cout << "COLOR: " << COLORS[to_move] << " MOVE: " << m.toString() << endl;
                cout << "MOVE: " << m.toString() << endl;
                print();
                while(!move_history.empty()) {
                    cout << "MOVE: " << move_history.top().toString() << endl;
                    cout << "CASTLING MASK: " << intToString(move_history.top().castling_mask) << endl;
                    undoLastMove();
                    print();
                }
            }*/
            swap(squares[CASTLE_LONG_ROOK_PATH[to_move][0]], squares[CASTLE_LONG_ROOK_PATH[to_move][1]]); //move rook            
            squares[CASTLE_LONG_ROOK_PATH[to_move][1]]->square = CASTLE_LONG_ROOK_PATH[to_move][1];
            break;
        case MOVETYPE_CASTLE_SHORT:
            swap(squares[CASTLE_SHORT_ROOK_PATH[to_move][0]], squares[CASTLE_SHORT_ROOK_PATH[to_move][1]]); //move rook
            squares[CASTLE_SHORT_ROOK_PATH[to_move][1]]->square = CASTLE_SHORT_ROOK_PATH[to_move][1];
            break;
        case MOVETYPE_EN_PASSENT:
            capture_square = m.to - PAWN_DELTAS[to_move][0];
            break;
        case MOVETYPE_ORDINARY:
            //if piece is a pawn of any color
            if((m.piece_type & MASK_PIECE) == PAWN) {
                //if pawn move was a two step move..
                uint8_t dist = abs((int)m.from - (int)m.to);
                if(dist == PAWN_DOUBLE_STEP_DIST) {
                    en_passent_square = m.from + PAWN_DELTAS[to_move][0];
                }
            }
            break;
        case MOVETYPE_PROMOTION_B:
            squares[m.to]->type = BISHOP + COLOR_PIECE_OFFSET[to_move];
            break;
        case MOVETYPE_PROMOTION_N:
            squares[m.to]->type = KNIGHT + COLOR_PIECE_OFFSET[to_move];
            break;
        case MOVETYPE_PROMOTION_Q:
            squares[m.to]->type = QUEEN + COLOR_PIECE_OFFSET[to_move];
            break;
        case MOVETYPE_PROMOTION_R:
            squares[m.to]->type = ROOK + COLOR_PIECE_OFFSET[to_move];
            break;
        default:
            break;
    }

    if(m.capture_type != EMPTY || ((m.piece_type & MASK_PIECE) == PAWN)) {
        //capture or pawn move -> reset 50 moves draw counter
        draw_count_history.push(0);
    } else {
        draw_count_history.push(draw_count_history.top() + 1);
    }

    //if a capture has occured.. remove captured piece
    if(m.capture_type != EMPTY) {
        squares[capture_square]->square = capture_square;
        //TODO..
        if(!removePiece(capture_square, pieces[(to_move+1) % 2])) { //; //capture
           // ERROR(moveToString(m));
        }

        //if a rook is captured.. check castling rights
        if((m.capture_type & MASK_PIECE) == ROOK) {
            uint8_t opp = (to_move + 1) % 2;

            if(castle_short[opp] && m.to == CASTLE_SHORT_ROOK_PATH[opp][0]) {
                m.castling_mask |= DISABLE_SHORT_OPP_CASTLING;
                castle_short[opp] = false;
            } else if(castle_long[opp] && m.to == CASTLE_LONG_ROOK_PATH[opp][0]) {
                m.castling_mask |= DISABLE_LONG_OPP_CASTLING;
                castle_long[opp] = false;
            }
        }
    }

    //castling rights
    //if king has moved, disable castling
    if((m.piece_type & MASK_PIECE) == KING) {
        king_square[to_move] = m.to;
        if(castle_short[to_move]) {
            m.castling_mask |= DISABLE_SHORT_CASTLING;
            castle_short[to_move] = false;
        }
        if(castle_long[to_move]) {
            m.castling_mask |= DISABLE_LONG_CASTLING;
            castle_long[to_move] = false;
        }
    }
    //check if rook has moved
    if((m.piece_type & MASK_PIECE) == ROOK) {
        if(m.from == CASTLE_SHORT_ROOK_PATH[to_move][0]) { //short
            if(castle_short[to_move]) {
                m.castling_mask |= DISABLE_SHORT_CASTLING;
                castle_short[to_move] = false;
            }
        } else if(m.from == CASTLE_LONG_ROOK_PATH[to_move][0]) { //long
            //cout << "LONG ROOK MOVED" << endl;
            if(castle_long[to_move]) {
                //cout << "DISABLE LONG CASTLING" << endl;
                m.castling_mask |= DISABLE_LONG_CASTLING;
                castle_long[to_move] = false;
            }
        }
    }

    //push move onto history stack
    move_history.push(m);

    //only count complete moves.. no half moves
    if(to_move == BLACK) {
        move_number++;
    }

    //change color
    to_move = (to_move + 1) % 2;
}

void Board0x88::genPseudoLegalMoves(vector<Move> &moves) {
    if(to_move == WHITE) {
        genPseudoLegalMovesForPieces(pieces[WHITE], moves);
    } else {
        genPseudoLegalMovesForPieces(pieces[BLACK], moves);
    }
}

void Board0x88::genPseudoLegalMovesForPieces(list<Piece> &pieces, vector<Move> &moves) {
    //for every piece
    for(Piece &p : pieces) {

        uint8_t raw_type = p.type & MASK_RAW_PIECE_TYPE;

        //test piece
        switch(raw_type) {
            case PAWN:
                genPseudoLegalPawnMoves(p.square, moves);
                break;
            case KNIGHT:
                genPseudoLegalKnightMoves(p.square, moves);
                break;
            case BISHOP:
                genPseudoLegalSlidingPieceMoves(p.square, BISHOP, moves, BISHOP_DELTAS, 4);
                break;
            case ROOK:
                genPseudoLegalSlidingPieceMoves(p.square, ROOK, moves, ROOK_DELTAS, 4);
                break;
            case QUEEN:
                genPseudoLegalSlidingPieceMoves(p.square, QUEEN, moves, QUEEN_DELTAS, 8);
                break;
            case KING:
                genPseudoLegalKingMoves(p.square, moves);
                break;
        }
    }
}

void Board0x88::genPseudoLegalKnightMoves(Index square, vector<Move> &moves) {
    Index target;
    Move m;

    for(const Index &d : KNIGHT_DELTAS) {
        target = square + d;

        //if target is not occupied by a piece of the same color
        if(isOnBoard(target)) {
            if(squares[target] == NULL) {
                //move
                m = Move(KNIGHT + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, EMPTY);
                moves.push_back(m);
            } else if(((squares[target]->type & MASK_COLOR) >> 3) != to_move) {
                //capture
                m = Move(KNIGHT + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, squares[target]->type);
                moves.push_back(m);
            }
        }
    }
}

//TODO -- gen pseudo sliding piece moves..
void Board0x88::genPseudoLegalSlidingPieceMoves(Index square, const uint8_t ptype, vector<Move> &moves, const Offset *slider_deltas, const int deltas_size) {
    Index target;
    Move m;

    for(int i = 0; i < deltas_size; i++) {
        const Index d = slider_deltas[i];
        target = square + d;
        //sliding piece, repeat until out of board
        while(isOnBoard(target)) {
            if(squares[target] == NULL) {
                //move
                m = Move(ptype + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, EMPTY);
                moves.push_back(m);
                //slide more
            } else if(((squares[target]->type & MASK_COLOR) >> 3) != to_move) {
                //capture
                m = Move(ptype + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, squares[target]->type);
                moves.push_back(m);
                break; //finish slide
            } else {
                break; //finish slide
            }
            target = target + d;
        }
    }
}

void Board0x88::genPseudoLegalPawnMoves(Index square, vector<Move> &moves) {
    Index target;
    Move m;

    //one step forward
    target = square + PAWN_DELTAS[to_move][0];
    if(squares[target] == NULL) { //while? //TODO
        //promotion?
        if(target >= PAWN_PROMOTION_RANGE[to_move][0] && target <= PAWN_PROMOTION_RANGE[to_move][1]) {
            m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_Q, EMPTY);
            moves.push_back(m);
            m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_R, EMPTY);
            moves.push_back(m);
            m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_B, EMPTY);
            moves.push_back(m);
            m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_N, EMPTY);
            moves.push_back(m);
            //break?//TODO
        } else { //normal advance
            m = Move(WHITE_PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, EMPTY);
            moves.push_back(m);
        }

        //check two steps forward // cant be
        if(square >= PAWN_DOUBLE_STEP_RANGE[to_move][0] && square <= PAWN_DOUBLE_STEP_RANGE[to_move][1]) {
            //pawn has not moved.. double jump is allowed
            target += PAWN_DELTAS[to_move][0]; //add one step
            if(squares[target] == NULL) {
                m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, EMPTY);
                moves.push_back(m);
            }
        }
        //TODO
    }

    //capture left & right
    for(int i = 1; i <= 2; i++) {
        target = square + PAWN_DELTAS[to_move][i];
        if(isOnBoard(target)) {
            if(squares[target] != NULL && (((squares[target]->type & MASK_COLOR) >> 3) != to_move)) {
                //promotion?
                if(target >= PAWN_PROMOTION_RANGE[to_move][0] && target <= PAWN_PROMOTION_RANGE[to_move][1]) {
                    m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_Q, squares[target]->type);
                    moves.push_back(m);
                    m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_R, squares[target]->type);
                    moves.push_back(m);
                    m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_B, squares[target]->type);
                    moves.push_back(m);
                    m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_PROMOTION_N, squares[target]->type);
                    moves.push_back(m);
                    continue; //nothing else can happen
                } else {
                    m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, squares[target]->type);
                    moves.push_back(m);
                }
            } else if(squares[target] == NULL && target == en_passent_square) {
                //en passent
                m = Move(PAWN + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_EN_PASSENT, squares[target+PAWN_DELTAS[(to_move+1) % 2][0]]->type);
                moves.push_back(m);
            }
        }
    }
}

void Board0x88::genPseudoLegalKingMoves(Index square, vector<Move> &moves) {
    Index target;
    Move m;

    //regular king moves
    for(const Index &d : KING_DELTAS) {
        target = square + d;

        //if target is not occupied by a piece of the same color
        if(isOnBoard(target)) {
            if(squares[target] == NULL) {
                //move
                m = Move(KING + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, EMPTY);
                moves.push_back(m);
            } else if(((squares[target]->type & MASK_COLOR) >> 3) != to_move) {
                //capture
                m = Move(KING + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_ORDINARY, squares[target]->type);
                moves.push_back(m);
            }
        }
    }

    //castling kingside
    if(castle_short[to_move] && squares[CASTLE_SHORT_PATH[to_move][0]] == NULL && squares[CASTLE_SHORT_PATH[to_move][1]] == NULL ) {
        target = CASTLE_SHORT_SQUARE[to_move];
        m = Move(KING + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_CASTLE_SHORT, EMPTY);
        moves.push_back(m);
    }
    //castling queenside
    if(castle_long[to_move] && squares[CASTLE_LONG_PATH[to_move][0]] == NULL && squares[CASTLE_LONG_PATH[to_move][1]] == NULL
            && squares[CASTLE_LONG_EXTRA_SQUARE[to_move]] == NULL) {
        target = CASTLE_LONG_SQUARE[to_move];
        m = Move(KING + COLOR_PIECE_OFFSET[to_move], square, target, MOVETYPE_CASTLE_LONG, EMPTY);
        moves.push_back(m);
    }
/*
    if(to_move == WHITE) {
        //castling kingside
        if(castle_short[WHITE] && squares[CASTLE_SHORT_PATH[WHITE][0]] == NULL && squares[CASTLE_SHORT_PATH[WHITE][1]] == NULL) {
            target = CASTLE_SHORT_SQUARE[WHITE];
            m = Move(WHITE_KING, square, target, MOVETYPE_CASTLE_SHORT, EMPTY);
            moves.push_back(m);
        }
        //castling queenside
        if(castle_long[WHITE] && squares[CASTLE_LONG_PATH[WHITE][0]] == NULL && squares[CASTLE_LONG_PATH[WHITE][1]] == NULL) {
            target = CASTLE_LONG_SQUARE[WHITE];
            m = Move(WHITE_KING, square, target, MOVETYPE_CASTLE_LONG, EMPTY);
            moves.push_back(m);
        }
    } else { //black
        //castling kingside
        if(castle_short[BLACK] && squares[CASTLE_SHORT_PATH[BLACK][0]] == NULL && squares[CASTLE_SHORT_PATH[BLACK][1]] == NULL) {
            target = CASTLE_SHORT_SQUARE[BLACK];
            m = Move(BLACK_KING, square, target, MOVETYPE_CASTLE_SHORT, EMPTY);
            moves.push_back(m);
        }
        //castling queenside
        if(castle_long[BLACK] && squares[CASTLE_LONG_PATH[BLACK][0]] == NULL && squares[CASTLE_LONG_PATH[BLACK][1]] == NULL) {
            target = CASTLE_LONG_SQUARE[BLACK];
            m = Move(BLACK_KING, square, target, MOVETYPE_CASTLE_LONG, EMPTY);
            moves.push_back(m);
        }
    }
*/
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
    if(en_passent_square != NO_EN_PASSENT) {
        char xx = CHESS_COORDS[en_passent_square % 16];
        Index yy = (en_passent_square / 16) + 1;
        fen += xx;
        fen += intToString(yy);
    } else {
        fen += "-";
    }
    fen += " ";

    //50 half moves until draw
    fen += intToString(draw_count_history.top()) + " ";

    //moves
    fen += intToString(move_number) + " ";

    return fen;
}

void Board0x88::setFENPosition(string fen) {
    Index x = 0;
    Index y = BOARD_SIZE - 1;
    istringstream iss(fen);
    pieces[WHITE].clear();
    pieces[BLACK].clear();

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
            en_passent_square = NO_EN_PASSENT;
        } else {
            for(int i = 0; i < ep.size(); i+=2) {
                if(ep[i] >= 'a' && ep[i] <= 'h' && ep[i+1] >= '1' && ep[i+1] <= '8') {
                    int x = ep[i] - 'a';
                    int y = ep[i+1] - '1';
                    en_passent_square = (int)(16*y + x);
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
        draw_count_history.push(atoi(s.c_str()));
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

void Board0x88::print() const {
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
    if(en_passent_square != NO_EN_PASSENT) {
        char xx = CHESS_COORDS[en_passent_square % 16];
        Index yy = (en_passent_square / 16) + 1;
        cout << xx << intToString(yy) << endl;
    } else {
        cout << "-" << endl;
    }

    cout << "   50 moves draw counter: " << intToString(draw_count_history.top()) << endl;

    cout << "   Next move number: " << intToString(move_number) << endl;

    cout << "   FEN-Code: " << getFENCode() << endl;
    cout << endl << "******************************************************************************" << endl << endl;
}

void Board0x88::setStartingPosition() {
    setFENPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

#endif // BOARD0X88_HPP