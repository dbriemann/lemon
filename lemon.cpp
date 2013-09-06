
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
    bboard.setStartingPosition();

    bboard.print();

    MoveList mlist;
    bboard.genKnightMoves(mlist);

    for(int i = 0; i < mlist.size; i++) {
        cout << moveToStr(mlist.moves[i]) << endl;
    }

    return 0;
}
