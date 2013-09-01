
#include <iostream>
#include <vector>
#include <cstdint>
using namespace std;

#include "Board.hpp"
#include "Board0x88.hpp"
#include "Move.hpp"
#include "Timers.hpp"

//TODO checks, checkmates..
static void perft(Board0x88 &board, Move *last_move, int depth, uint64_t &nodes, uint64_t &captures, uint64_t &enpassents, uint64_t &castles, uint64_t &promotions) {
    vector<Move> moves;

    if(depth == 0) {
        if(last_move->capture_type != EMPTY) {
            captures++;
        }
        if(last_move->move_type == MOVETYPE_EN_PASSENT) {
            enpassents++;
        } else if(last_move->move_type == MOVETYPE_CASTLE_LONG || last_move->move_type == MOVETYPE_CASTLE_SHORT) {
            //cout << "COLOR: " << COLORS[(board.to_move+1) % 2] << " MOVE: " << last_move->toString() << endl;
            //board.print();
            castles++;
        } else if(last_move->move_type == MOVETYPE_PROMOTION_B || last_move->move_type == MOVETYPE_PROMOTION_N
                  || last_move->move_type == MOVETYPE_PROMOTION_Q || last_move->move_type == MOVETYPE_PROMOTION_R) {
            promotions++;
        }

        nodes++;
    } else {
        moves.clear();
        board.genPseudoLegalMoves(moves);

        for(Move &m : moves) {
            if(board.makeMoveIfLegal(m)) {
                perft(board, &m, depth-1, nodes, captures, enpassents, castles, promotions);
                board.undoLastMove();
            }
        }

    }
}

static void testBoard(Board0x88 &board, string fen, int maxdepth) {
    cout << "***PERFT***" << endl;
    cout << "FEN: " << fen << endl;
    board.setFENPosition(fen);

    for(int i = 1; i <= maxdepth; i++) {
        uint64_t nodes = 0;
        uint64_t captures = 0;
        uint64_t enpassents = 0;
        uint64_t castles = 0;
        uint64_t promotions = 0;

        perft(board, NULL, i, nodes, captures, enpassents, castles, promotions);

        cout << "Depth: " << i
             << " Nodes: " << nodes
             << " Captures: " << captures
             << " En Passent: " << enpassents
             << " Castles: " << castles
             << " Promotions: " << promotions << endl;
    }

    cout << "*********" << endl << endl;
}

int main() {
    Board0x88 board;

    //starting position
    //OK
    //testBoard(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 7);

    //TODO FAIL
    //testBoard(board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5);

    //TODO FAIL
    //testBoard(board, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 7);

    //TODO FAIL
    //testBoard(board, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6);

    return 0;
}
