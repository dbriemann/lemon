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
    //U8 occupancy[64];
    U8 kings[2];
    bool is_check;

    //wrap all following into one U32?
    U8 player;
    bool castle_short[2];
    bool castle_long[2];
    Square en_passent_sq;
    U8 draw_counter;
    U8 move_number;


    /*
     * Constructors, copy-constructor, destructor
     * move constructor?, move assignment operator?
     * operators and related stuff
     */
    BitBoard();
    BitBoard(const BitBoard &other);
    BitBoard& operator=(const BitBoard &other);



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
    void genPseudoLegalMoves(MoveList &mlist);
    void genKnightMoves(MoveList &mlist);
    void genPawnMoves(MoveList &mlist);
    void genKingMoves(MoveList &mlist);
    void genRookMoves(MoveList &mlist);
    void genBishopMoves(MoveList &mlist);
    void genQueenMoves(MoveList &mlist);

    U64 genRankAttacks(const U64 occ, const U8 sq);
    U64 genFileAttacks(U64 occ, const U8 sq);
    U64 genDiagAttacks(U64 occ, const U8 sq);
    U64 genAntiDiagAttacks(U64 occ, const U8 sq);

    /*
     * Move execution methods
     */
    bool makeLightMove(Move m);
    //bool makeMoveIfLegal(Move m);
    U64 isAttackedBy(const U64 targets_bb, const U8 atk_color);


};


BitBoard::BitBoard() {
    zeroAll(); //bitboards
    player = WHITE;
    castle_long[WHITE] = castle_long[BLACK] = castle_short[WHITE] = castle_short[BLACK] = true;
    kings[WHITE] = kings[BLACK] = 0;
    en_passent_sq = NONE;
    draw_counter = 0;
    move_number = 1;
    is_check = false;
//    for(int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
//        this->occupancy[i] = EMPTY;
//    }
}

BitBoard::BitBoard(const BitBoard& other) {
    for(int c = WHITE; c <= BLACK; c++) {
        this->pieces_by_color_bb[c] = other.pieces_by_color_bb[c];
        this->castle_long[c] = other.castle_long[c];
        this->castle_short[c] = other.castle_short[c];
        this->kings[c] = other.kings[c];
    }
    for(int t = PAWN; t <= KING; t++) {
        this->pieces_by_type_bb[t] = other.pieces_by_type_bb[t];
    }
    this->is_check = other.is_check;
    this->en_passent_sq = other.en_passent_sq;
    this->draw_counter = other.draw_counter;
    this->move_number = other.move_number;
    this->player = other.player;
//    for(int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
//        this->occupancy[i] = other.occupancy[i];
//    }
}


BitBoard& BitBoard::operator=(const BitBoard& other) {
    for(int c = WHITE; c <= BLACK; c++) {
        this->pieces_by_color_bb[c] = other.pieces_by_color_bb[c];
        this->castle_long[c] = other.castle_long[c];
        this->castle_short[c] = other.castle_short[c];
        this->kings[c] = other.kings[c];
    }
    for(int t = PAWN; t <= KING; t++) {
        this->pieces_by_type_bb[t] = other.pieces_by_type_bb[t];
    }
    this->en_passent_sq = other.en_passent_sq;
    this->draw_counter = other.draw_counter;
    this->move_number = other.move_number;
    this->player = other.player;
    this->is_check = other.is_check;
//    for(int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
//        this->occupancy[i] = other.occupancy[i];
//    }
    return *this;
}

inline
void BitBoard::zeroAll() {
    pieces_by_color_bb[WHITE] = pieces_by_color_bb[BLACK] = C64(0);
    for(int i = PAWN; i <= KING; i++) {
        pieces_by_type_bb[i] = C64(0);
    }
//    for(int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
//        occupancy[i] = EMPTY;
//    }
}



/*
 * executes a move and checks for legality
 * returns true if the move was legal and executed properly
 * returns false if the move was not legal. in this case
 * the board will remain in an undefined/broken state
 * user has to make sure the board gets replaced with a proper
 * valid board.
 * works only with copy/make functionality. remains in illegal
 * state to increase performance.
 */
bool BitBoard::makeLightMove(Move m) {
    //cout << "Try move: " << moveToStr(m) << endl;
    /*
     * extraction of move features
     */
    const U8 opponent = FLIP(player);
    const U32 from = moveGetFeature(m, FROM_MASK, FROM_SHIFT);
    const U32 to = moveGetFeature(m, TO_MASK, TO_SHIFT);
    const U32 ptype = moveGetFeature(m, PIECE_MASK, PIECE_SHIFT);
    const U32 capflag = moveGetFeature(m, CAPTURE_MASK, CAPTURE_SHIFT);
    const U32 eptype = moveGetFeature(m, EP_MASK, EP_SHIFT);
    const U32 promtype = moveGetFeature(m, PROMOTION_MASK, PROMOTION_SHIFT);
    //reset ep square
    //const U8 target_piece = occupancy[to];
    en_passent_sq = NONE;
    U64 check_bb = 0;


    /*
     * begin moving the piece by removing it from the
     * <from> position of its corresponding bbs
     * then swap <from> and <to> positions on the
     * occupancy helper board
     */
    pieces_by_color_bb[player] &= ~iBitMask(from);
    pieces_by_type_bb[ptype] &= ~iBitMask(from);
    //swap(occupancy[from], occupancy[to]);

    /*
     * in case of a capture remove the
     * opponent's piece from its bbs
     */
    if(capflag) {
        if(eptype == EP_TYPE_CAPTURE) {
            //captured pawn is one step ahead of <to>
            U8 pbit = to - PAWN_MOVE_DIRECTIONS[player];
            pieces_by_color_bb[opponent] &= ~iBitMask(pbit);
            //pieces_by_type_bb[occupancy[pbit]] &= ~iBitMask(pbit); //already swapped..from<->to
            pieces_by_type_bb[PAWN] &= ~iBitMask(pbit); //already swapped..from<->to
            //occupancy[pbit] = EMPTY;
        } else {
            U64 kill_bb = ~iBitMask(to);
            pieces_by_color_bb[opponent] &= kill_bb;
            //pieces_by_type_bb[occupancy[from]] &= ~iBitMask(to); //already swapped..from<->to
            pieces_by_type_bb[PAWN] &= kill_bb;
            pieces_by_type_bb[KNIGHT] &= kill_bb;
            pieces_by_type_bb[BISHOP] &= kill_bb;
            pieces_by_type_bb[ROOK] &= kill_bb;
            pieces_by_type_bb[QUEEN] &= kill_bb;

            //check if capture disables opponent
            //from future castling by capturing a rook
            //if(target_piece == ROOK) {
            if(to == CASTLE_SHORT_ROOK[opponent]) {
                castle_short[opponent] = false;
            } else if(to == CASTLE_LONG_ROOK[opponent]) {
                castle_long[opponent] = false;
            }
            //}
        }

        //reset draw counter
        draw_counter = 0;
    } else {
        //special non capture moves
        if(ptype == PAWN) {
            draw_counter = 0;
            if(eptype == EP_TYPE_CREATE) {
                en_passent_sq = to - PAWN_MOVE_DIRECTIONS[player];
            }
        } else if(ptype == KING) {
            const U32 castleflag = moveGetFeature(m, CASTLE_MASK, CASTLE_SHIFT);
            if(castleflag) {
                //TODO check squares for attacks + check
                //TODO castling flag -> castling type??
                if(to == CASTLE_SHORT_TARGET[player]) {
                    check_bb |= CASTLE_SHORT_PATH[player];
                    if(is_check || isAttackedBy(check_bb, FLIP(player))) {
                        //cout << "CASTLE SHORT CANCEL" << endl;
                        return false;
                    }
                    //castle short
                    //move rook..
                    //swap(occupancy[CASTLE_SHORT_ROOK[player]], occupancy[CASTLE_SHORT_ROOK_TARGET[player]]);
                    pieces_by_color_bb[player] &= ~iBitMask(CASTLE_SHORT_ROOK[player]);
                    pieces_by_color_bb[player] |= iBitMask(CASTLE_SHORT_ROOK_TARGET[player]);
                    pieces_by_type_bb[ROOK] &= ~iBitMask(CASTLE_SHORT_ROOK[player]);
                    pieces_by_type_bb[ROOK] |= iBitMask(CASTLE_SHORT_ROOK_TARGET[player]);
                } else {
                    check_bb |= CASTLE_LONG_CHECK_PATH[player];
                    if(is_check || isAttackedBy(check_bb, FLIP(player))) {
                        //cout << "CASTLE LONG CANCEL" << endl;
                        return false;
                    }
                    //castle long
                    //move rook..
                    //swap(occupancy[CASTLE_LONG_ROOK[player]], occupancy[CASTLE_LONG_ROOK_TARGET[player]]);
                    pieces_by_color_bb[player] &= ~iBitMask(CASTLE_LONG_ROOK[player]);
                    pieces_by_color_bb[player] |= iBitMask(CASTLE_LONG_ROOK_TARGET[player]);
                    pieces_by_type_bb[ROOK] &= ~iBitMask(CASTLE_LONG_ROOK[player]);
                    pieces_by_type_bb[ROOK] |= iBitMask(CASTLE_LONG_ROOK_TARGET[player]);
                }
            }
        }
    }

    if(ptype == ROOK) {
        //disable castling on rook moves
        if(from == CASTLE_SHORT_ROOK[player]) {
            castle_short[player] = false;
        } else if(from == CASTLE_LONG_ROOK[player]) {
            castle_long[player] = false;
        }
    } else if(ptype == KING) {
        kings[player] = to;
        //king moves always disable all castling rights
        castle_long[player] = false;
        castle_short[player] = false;
    }

    /*
     * finally add the moving piece to its destination
     * position on its corresponding bbs
     */
    pieces_by_color_bb[player] |= iBitMask(to);
    pieces_by_type_bb[ptype] |= iBitMask(to);

    //promotion..
    if(promtype != EMPTY) {
        //occupancy[to] = promtype;
        pieces_by_type_bb[PAWN] &= ~iBitMask(to);
        pieces_by_type_bb[promtype] |= iBitMask(to);
    }

    check_bb = iBitMask(kings[player]);
    if(isAttackedBy(check_bb, FLIP(player))) {
        //cout << "ATTACK CHECK CANCEL" << endl;
        return false;
    }

    /*
     * finish everything
     */
    //delete from helper board
    //occupancy[from] = EMPTY; //already swapped..from<->to
    //switch player
    player = FLIP(player);
    if(player == WHITE) {
        move_number++;
    }

    //determine check status
    if(isAttackedBy(iBitMask(kings[player]), FLIP(player))) {
        is_check = true;
    } else {
        is_check = false;
    }

    return true;
}


//bool BitBoard::makeMoveIfLegal(Move m) {
//    /*
//     * temporary variables
//     * and extraction of move features
//     */
//    bool success = true;
//    const U8 opponent = FLIP(player);
//    const U32 from = moveGetFeature(m, FROM_MASK, FROM_SHIFT);
//    const U32 to = moveGetFeature(m, TO_MASK, TO_SHIFT);
//    const U32 ptype = moveGetFeature(m, PIECE_MASK, PIECE_SHIFT);
//    const U32 capflag = moveGetFeature(m, CAPTURE_MASK, CAPTURE_SHIFT);
//    const U32 eptype = moveGetFeature(m, EP_MASK, EP_SHIFT);
//    const U32 promtype = moveGetFeature(m, PROMOTION_MASK, PROMOTION_SHIFT);

//    /*
//     * backup game state specific variables
//     * in case the move is illegal and
//     * must be reverted in place
//     */
//    bool castle_000[2];
//    castle_000[WHITE] = castle_long[WHITE];
//    castle_000[BLACK] = castle_long[BLACK];
//    bool castle_00[2];
//    castle_00[WHITE] = castle_short[WHITE];
//    castle_00[BLACK] = castle_short[BLACK];
//    U8 draw = draw_counter;
//    U8 ep = en_passent_sq;
//    //remember piece types?
//    //const U8 from_piece = occupancy[from];
//    const U8 to_piece = occupancy[to];
//    //cout << intToString(ptype) << " " << intToString(to_piece) << endl;

//    //reset ep square
//    en_passent_sq = NONE;

//    /*
//     * begin moving the piece by removing it from the
//     * <from> position of its corresponding bbs
//     * then swap <from> and <to> positions on the
//     * occupancy helper board
//     */
//    pieces_by_color_bb[player] &= ~iBitMask(from);
//    pieces_by_type_bb[ptype] &= ~iBitMask(from);
//    swap(occupancy[from], occupancy[to]);

//    /*
//     * in case of a capture remove the
//     * opponent's piece from its bbs
//     */
//    if(capflag) {
//        if(eptype == EP_TYPE_CAPTURE) {
//            //captured pawn is one step ahead of <to>
//            U8 pbit = to - PAWN_MOVE_DIRECTIONS[player];
//            pieces_by_color_bb[opponent] &= ~iBitMask(pbit);
//            pieces_by_type_bb[occupancy[pbit]] &= ~iBitMask(pbit); //already swapped..from<->to
//            occupancy[pbit] = EMPTY;
//        } else {
//            pieces_by_color_bb[opponent] &= ~iBitMask(to);
//            pieces_by_type_bb[occupancy[from]] &= ~iBitMask(to); //already swapped..from<->to

//            //check if capture disables opponent
//            //from future castling by capturing a rook
//            if(to_piece == ROOK) {
//                if(to == CASTLE_SHORT_ROOK[opponent]) {
//                    castle_short[opponent] = false;
//                } else if(to == CASTLE_LONG_ROOK[opponent]) {
//                    castle_long[opponent] = false;
//                }
//            }
//        }

//        //reset draw counter
//        draw_counter = 0;
//    } else {
//        const U32 castleflag = moveGetFeature(m, CASTLE_MASK, CASTLE_SHIFT);

//        //special non capture moves
//        if(eptype == EP_TYPE_CREATE) {
//            en_passent_sq = to - PAWN_MOVE_DIRECTIONS[player];
//        } else if(ptype == KING) {
//            if(castleflag) {
//                //TODO check squares for attacks + check
//                //TODO castling flag -> castling type??
//                if(to == CASTLE_SHORT_TARGET[player]) {
//                    //castle short
//                    //move rook..
//                    swap(occupancy[CASTLE_SHORT_ROOK[player]], occupancy[CASTLE_SHORT_ROOK_TARGET[player]]);
//                    pieces_by_color_bb[player] &= ~iBitMask(occupancy[CASTLE_SHORT_ROOK[player]]);
//                    pieces_by_color_bb[player] |= iBitMask(occupancy[CASTLE_SHORT_ROOK_TARGET[player]]);
//                    pieces_by_type_bb[ROOK] &= ~iBitMask(occupancy[CASTLE_SHORT_ROOK[player]]);
//                    pieces_by_type_bb[ROOK] |= iBitMask(occupancy[CASTLE_SHORT_ROOK_TARGET[player]]);
//                } else {
//                    //castle long
//                    //move rook..
//                    swap(occupancy[CASTLE_LONG_ROOK[player]], occupancy[CASTLE_LONG_ROOK_TARGET[player]]);
//                    pieces_by_color_bb[player] &= ~iBitMask(occupancy[CASTLE_LONG_ROOK[player]]);
//                    pieces_by_color_bb[player] |= iBitMask(occupancy[CASTLE_LONG_ROOK_TARGET[player]]);
//                    pieces_by_type_bb[ROOK] &= ~iBitMask(occupancy[CASTLE_LONG_ROOK[player]]);
//                    pieces_by_type_bb[ROOK] |= iBitMask(occupancy[CASTLE_LONG_ROOK_TARGET[player]]);
//                }
//            }

//            kings[player] = to;

//            //king moves always disable all castling rights
//            castle_long[player] = false;
//            castle_short[player] = false;
//        }
//    }

//    /*
//     * finally add the moving piece to its destination
//     * position on its corresponding bbs
//     */
//    pieces_by_color_bb[player] |= iBitMask(to);
//    pieces_by_type_bb[ptype] |= iBitMask(to);

//    //promotion..
//    if(promtype != EMPTY) {
//        occupancy[to] = promtype;
//        pieces_by_type_bb[PAWN] &= ~iBitMask(to);
//        pieces_by_type_bb[promtype] |= iBitMask(to);
//    }

//    if(success) {
//        //delete from helper board
//        occupancy[from] = EMPTY; //already swapped..from<->to
//        //switch player
//        player = FLIP(player);
//        if(player == WHITE) {
//            move_number++;
//        }
//    } else {
//        //revert everything to backups
//        draw_counter = draw;
//        en_passent_sq = ep;
//        castle_long[WHITE] = castle_000[WHITE];
//        castle_long[BLACK] = castle_000[BLACK];
//        castle_short[WHITE] = castle_00[WHITE];
//        castle_short[BLACK] = castle_00[BLACK];
//        occupancy[from] = ptype;
//        occupancy[to] = to_piece;
//    }


//    return success;
//}


//TODO: merge all movegen functions into one big?
//or inline or #define stuff
void BitBoard::genPseudoLegalMoves(MoveList &mlist) {
    //TODO
//    if(is_check) {
//        ERROR("CHECK MOVE GEN");
//    } else {
        genQueenMoves(mlist);
        genRookMoves(mlist);
        genBishopMoves(mlist);
        genKnightMoves(mlist);
        genPawnMoves(mlist);
        genKingMoves(mlist);
//    }
}

inline
void BitBoard::genKingMoves(MoveList &mlist) {
    register U64 bb;
    const U64 occ_bb = pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK];
    const U64 unocc_bb = ~occ_bb;
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    Move m = 0;
    U32 from, to;

    from = kings[player];
    bb = KING_TARGET_BBS[from] & unocc_bb;
    //normal moves
    while(bb) {
        to = bitscanfwd(bb);

        m = moveCreate(from, to, KING, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT);
        mlist.put(m);

        bb &= ~iBitMask(to);
    }

    bb = KING_TARGET_BBS[from] & opp_bb;
    //captures
    while(bb) {
        to = bitscanfwd(bb);

        m = moveCreate(from, to, KING, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT);
        mlist.put(m);

        bb &= ~iBitMask(to);
    }

    //castling
    if(castle_short[player] && !(occ_bb & CASTLE_SHORT_PATH[player])) {
        //castling short is possible
        m = moveCreate(from, CASTLE_SHORT_TARGET[player], KING, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_YES, MOVE_PRE_EVAL_CASTLE);
        mlist.put(m);
    }

    if(castle_long[player] && !(occ_bb & CASTLE_LONG_PATH[player])) {
        //castling long is possible
        m = moveCreate(from, CASTLE_LONG_TARGET[player], KING, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_YES, MOVE_PRE_EVAL_CASTLE);
        mlist.put(m);
    }
}

U64 BitBoard::genRankAttacks(const U64 occ, const U8 sq) {
    //TODO.. U32??
    U32 f = sq & 7;
    U32 r = sq & ~7; //== rank*8
    U32 o = (U32)(occ >> (r+1)) & 63;
    return ((U64) RANK_ATTACK_BBS[f][o]) << r;
}

U64 BitBoard::genFileAttacks(U64 occ, const U8 sq) {
    //TODO.. U32??
    U32 f = sq & 7;
    occ = FILE_A & (occ >> f);
    U32 o = (MAGIC_DIAGONAL_C2H7 * occ) >> 58;
    return FILE_ATTACK_BBS[RANK(sq)][o] << f;
}

U64 BitBoard::genDiagAttacks(U64 occ, const U8 sq) {
    occ &= A1H8_DIAGONAL_TARGET_BBS[sq];
    occ = (occ * FILE_B) >> 58;
    return A1H8_DIAGONAL_TARGET_BBS[sq] & FILLUP_ATTACK_BBS[sq&7][occ];
}

U64 BitBoard::genAntiDiagAttacks(U64 occ, const U8 sq) {
    occ &= H1A8_DIAGONAL_TARGET_BBS[sq];
    occ = (occ * FILE_B) >> 58;
    return H1A8_DIAGONAL_TARGET_BBS[sq] & FILLUP_ATTACK_BBS[sq&7][occ];
}

void BitBoard::genQueenMoves(MoveList &mlist) {
    register U64 bb;
    const U64 occ_bb = pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK];
    const U64 own_bb = pieces_by_color_bb[player];
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    U64 pieces_to_move_bb = pieces_by_type_bb[QUEEN] & own_bb;
    U64 captures_bb;
    U32 from,to;
    Move m = 0;

    while(pieces_to_move_bb) {
        from = bitscanfwd(pieces_to_move_bb);

        bb = genDiagAttacks(occ_bb, from);
        bb |= genAntiDiagAttacks(occ_bb, from);
        bb |= genRankAttacks(occ_bb, from);
        bb |= genFileAttacks(occ_bb, from);
        bb &= ~own_bb;
        captures_bb = bb & opp_bb;
        bb &= ~opp_bb;

        //normal moves
        while(bb) {
            to = bitscanfwd(bb);

            m = moveCreate(from, to, QUEEN, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        bb = captures_bb;
        //captures
        while(bb) {
            to = bitscanfwd(bb);

            m = moveCreate(from, to, QUEEN, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }


        pieces_to_move_bb &= ~iBitMask(from);
    }
}

void BitBoard::genBishopMoves(MoveList &mlist) {
    register U64 bb;
    const U64 occ_bb = pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK];
    const U64 own_bb = pieces_by_color_bb[player];
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    U64 pieces_to_move_bb = pieces_by_type_bb[BISHOP] & own_bb;
    U64 captures_bb;
    U32 from,to;
    Move m = 0;

    while(pieces_to_move_bb) {
        from = bitscanfwd(pieces_to_move_bb);

        bb = genDiagAttacks(occ_bb, from);
        bb |= genAntiDiagAttacks(occ_bb, from);
        bb &= ~own_bb;
        captures_bb = bb & opp_bb;
        bb &= ~opp_bb;

        //normal moves
        while(bb) {
            to = bitscanfwd(bb);

            m = moveCreate(from, to, BISHOP, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        bb = captures_bb;
        //captures
        while(bb) {
            to = bitscanfwd(bb);

            m = moveCreate(from, to, BISHOP, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_GOODCAP); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }


        pieces_to_move_bb &= ~iBitMask(from);
    }
}

void BitBoard::genRookMoves(MoveList &mlist) {
    register U64 bb;
    const U64 occ_bb = pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK];
    const U64 own_bb = pieces_by_color_bb[player];
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    U64 play_rooks_bb = pieces_by_type_bb[ROOK] & own_bb;
    U64 captures_bb;
    U32 from, to;
    Move m = 0;

    while(play_rooks_bb) {
        from = bitscanfwd(play_rooks_bb);

        bb = genRankAttacks(occ_bb, from);
        bb |= genFileAttacks(occ_bb, from);
        bb &= ~own_bb;
        captures_bb = bb & opp_bb;
        bb &= ~opp_bb;

        //normal moves
        while(bb) {
            to = bitscanfwd(bb);

            m = moveCreate(from, to, ROOK, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        bb = captures_bb;
        //captures
        while(bb) {
            to = bitscanfwd(bb);

            m = moveCreate(from, to, ROOK, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_GOODCAP); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        play_rooks_bb &= ~iBitMask(from);
    }
}


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

            m = moveCreate(from, to, KNIGHT, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT); //TODO VALUE
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

            m = moveCreate(from, to, KNIGHT, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_GOODCAP); //TODO VALUE
            mlist.put(m);

            bb &= ~iBitMask(to);
        }

        play_knights_bb &= ~iBitMask(from);
    }
}

/*
 * checks if any of the squares in targets_bb are attacked
 * by the pieces of player atk_color
 * returns a bitboard with the attacker's bits set, 0 == no attacker
 */

U64 BitBoard::isAttackedBy(const U64 targets_bb, const U8 atk_color) {
    register U64 opp_bb = pieces_by_color_bb[atk_color] & pieces_by_type_bb[KNIGHT];
    register U64 bb = targets_bb;
    register U64 check;
    const U64 occ_bb = pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK];
    U8 sq;
    //test all opponent pieces for attacks on targets_bb
    //treat targets as knights
    while(bb) {
        sq = bitscanfwd(bb);
        check = KNIGHT_TARGET_BBS[sq] & opp_bb;
        if(check) {
            //ERROR("KNIGHT CHECKS FROM " + intToString(sq));
            //PRINTBB(check, "KNIGHT ATTACK");
            return check;
        }

        bb &= ~iBitMask(sq);
    }

    //treat targets as bishops //include queen
    bb = targets_bb;
    opp_bb = pieces_by_color_bb[atk_color] & (pieces_by_type_bb[BISHOP] | pieces_by_type_bb[QUEEN]);
    while(bb) {
        sq = bitscanfwd(bb);

        //PRINTBB(genDiagAttacks(occ_bb, sq), "DIAG");
        //PRINTBB(genAntiDiagAttacks(occ_bb, sq), "ANTI-DIAG");

        check = (genDiagAttacks(occ_bb, sq) | genAntiDiagAttacks(occ_bb, sq)) & opp_bb;
        if(check) {
            //ERROR("BISHOP/QUEEN CHECKS FROM " + intToString(sq));
            //PRINTBB(check, "BISHOP/QUEEN ATTACK");
            return check;
        }
        bb &= ~iBitMask(sq);
    }

    //treat targets as rooks //include queen
    bb = targets_bb;
    opp_bb = pieces_by_color_bb[atk_color] & (pieces_by_type_bb[ROOK] | pieces_by_type_bb[QUEEN]);
    while(bb) {
        sq = bitscanfwd(bb);

        check = (genRankAttacks(occ_bb, sq) | genFileAttacks(occ_bb, sq)) & opp_bb;
        if(check) {
            //ERROR("ROOK/QUEEN CHECKS FROM " + intToString(sq));
            //PRINTBB(check, "ROOK/QUEEN ATTACK");
            return check;
        }
        bb &= ~iBitMask(sq);
    }

    //treat targets as pawns
    opp_bb = pieces_by_color_bb[atk_color] & pieces_by_type_bb[PAWN];
    bb = targets_bb;

    if(atk_color == WHITE) {
        bb = (_SHIFT_SE(bb) & (opp_bb & ~FILE_A)) | (_SHIFT_SW(bb) & (opp_bb & ~FILE_H));
    } else {
        bb = (_SHIFT_NE(bb) & (opp_bb & ~FILE_A)) | (_SHIFT_NW(bb) & (opp_bb & ~FILE_H));
    }


    if(bb) {
        //found pawn attacks on targets
        //ERROR("PAWN CHECKS FROM " + intToString(sq));
        //PRINTBB(check, "PAWN ATTACK");
        return bb;
    }

    //treat targets as kings
    //opp_bb = pieces_by_color_bb[atk_color] | pieces_by_type_bb[KING];
    bb = KING_TARGET_BBS[kings[atk_color]] & targets_bb;
    if(bb) {
        //ERROR("KING CHECKS FROM " + intToString(sq));
        //PRINTBB(check, "KING ATTACK");
        return bb;
    }

    return false;
}


void BitBoard::genPawnMoves(MoveList &mlist) {
    const U64 unocc_bb = ~(pieces_by_color_bb[WHITE] | pieces_by_color_bb[BLACK]);
    const U64 opp_bb = pieces_by_color_bb[FLIP(player)];
    const U64 own_bb = pieces_by_color_bb[player] & pieces_by_type_bb[PAWN];
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
            m = moveCreate(from, to, PAWN, CAPTURE_NO, QUEEN, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_NO, ROOK, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_NO, BISHOP, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_NO, KNIGHT, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMO);
            mlist.put(m);
        } else {
            m = moveCreate(from, to, PAWN, CAPTURE_NO, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT);
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

        m = moveCreate(from, to, PAWN, CAPTURE_NO, EMPTY, EP_TYPE_CREATE, CASTLE_NO, MOVE_PRE_EVAL_DEFAULT);
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
            m = moveCreate(from, to, PAWN, CAPTURE_YES, QUEEN, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, ROOK, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, BISHOP, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, KNIGHT, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
        } else {
            m = moveCreate(from, to, PAWN, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_GOODCAP);
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
            m = moveCreate(from, to, PAWN, CAPTURE_YES, QUEEN, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, ROOK, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, BISHOP, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
            m = moveCreate(from, to, PAWN, CAPTURE_YES, KNIGHT, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_PROMOCAP);
            mlist.put(m);
        } else {
            m = moveCreate(from, to, PAWN, CAPTURE_YES, EMPTY, EP_TYPE_NONE, CASTLE_NO, MOVE_PRE_EVAL_GOODCAP);
            mlist.put(m);
        }

        bb &= ~iBitMask(to);
    }

    //if there is a current ep. square check if pawns can capture there
    if(en_passent_sq) {
        bb = ep_takers;

        while(bb) {
            from = bitscanfwd(bb);

            m = moveCreate(from, en_passent_sq, PAWN, CAPTURE_YES, EMPTY, EP_TYPE_CAPTURE, CASTLE_NO, MOVE_PRE_EVAL_GOODCAP);
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


    //TODO remove debug overhead
    //piece = occupancy[bit];

    for(int p = PAWN; p <= KING; p++) {
        if(pieces_by_type_bb[p] & iBitMask(bit)) {
            piece = p;
            break;
        }
    }

//    if(piece != occupancy[bit]) {
//        ERROR("BitBoard::get() : boards not synced...");
//        exit(1); //BANG BANG
//    }


    return piece | color_offset;
}

void BitBoard::set(Square line, Square rank, OccupancyType type) {
    const U8 bit = 8*rank + line; //8*y + x == bit position in U64

    if(type == EMPTY) {
        //occupancy[bit] = EMPTY;
        U64 rmask = ~(iBitMask(bit));
        pieces_by_color_bb[WHITE] &= rmask;
        pieces_by_color_bb[BLACK] &= rmask;
        for(int p = PAWN; p <= KING; p++) {
            pieces_by_type_bb[p] &= rmask;
        }
    } else {
        //occupancy[bit] = (type & MASK_PIECE);
        U64 amask = iBitMask(bit);
        pieces_by_type_bb[type & MASK_PIECE] |= amask;
        U8 color = (type & MASK_COLOR) >> 3;
        pieces_by_color_bb[color] |= iBitMask(bit);

        if((type & MASK_PIECE) == KING) {
            kings[(type & MASK_COLOR) >> 3] = bit;
        }
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
            for(U32 i = 0; i < ep.size(); i+=2) {
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
