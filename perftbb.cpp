
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
using namespace std;

#include "BitBoard.hpp"
#include "Timers.hpp"
#include "MoveList.hpp"
#include "Move.hpp"

static CPUTimer timer;

U64 simplePerft(BitBoard &board, int depth, int maxdepth) {
    MoveList mlist;
    U64 nodes = 0;

    if (depth == maxdepth) return 1;

    BitBoard current_board(board);
    current_board.genPseudoLegalMoves(mlist);

    for(int i = 0; i < mlist.size; i++) {
        if(current_board.makeLightMove(mlist.moves[i])) {
            nodes += simplePerft(current_board, depth + 1, maxdepth);
        }
        current_board = board; //undo
    }

    return nodes;
}

int main() {
    BitBoard board;
    //board.setStartingPosition();
    board.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    for(int d = 1; d <= 5; d++) {
        timer.start();
        int p = simplePerft(board, 0, d);
        double t = timer.get_elapsed();
        cout << "depth=" << d << " | perft=" << p << " | time=" << fixed << t << " | sec. | nsec=" << (double)p/t << endl;
    }

    return 0;
}
