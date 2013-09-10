
#include <iostream>
#include <vector>
#include <cstdio>
#include <ctime>
using namespace std;

#include "BitBoard.hpp"
#include "MoveList.hpp"

int main() {
    srand(time(0));

    BitBoard bboard;
    //bboard.setStartingPosition();
    bboard.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    //bboard.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");


    //bboard.setFENPosition("6nk/1R2p3/8/4p1R1/8/6p1/8/1K6 w - - 0 1");
    //bboard.setFENPosition("3k4/2b5/4b3/8/8/3B4/8/B5K1 w - - 0 1");
    //bboard.setFENPosition("3k4/2b5/4b3/8/8/3B4/8/B5K1 b - - 0 1");


    bboard.print();
    //bboard.player = BLACK;

    MoveList mlist;
    bboard.genPseudoLegalMoves(mlist);

    for(int i = 0; i < mlist.size; i++) {
        cout << moveToStr(mlist.moves[i]) << ", ";
    }
    cout << endl;

    bboard.makeMoveIfLegal(mlist.moves[8]);
    bboard.print();

    return 0;
}
