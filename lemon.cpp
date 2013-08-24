
#include <iostream>
using namespace std;

#include "Board.hpp"
#include "Board0x88.hpp"
#include "Move.hpp"

int main() {

    Board *board = new Board0x88();
    //board->setStartingPosition();
    //board->print();
    //board->setFENPosition("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
    board->setFENPosition("r1b2bnr/1pqkpPpp/p1n5/1Bpp4/8/5N2/PPPP1PPP/RNBQK2R w KQ - 0 7");
    board->print();
    list<Move> moves = board->genPseudoLegalMoves();

    for(Move m : moves) {
        cout << moveToString(m) << ", ";
    }
    cout << endl;


    return 0;
}
