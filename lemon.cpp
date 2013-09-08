
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


    bboard.print();
    //bboard.player = BLACK;

    MoveList mlist;
    bboard.genKnightMoves(mlist);
    bboard.genPawnMoves(mlist);
    bboard.genKingMoves(mlist);

    for(int i = 0; i < mlist.size; i++) {
        cout << moveToStr(mlist.moves[i]) << endl;
    }

    return 0;
}
