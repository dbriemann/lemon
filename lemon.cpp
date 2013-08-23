
#include <iostream>
using namespace std;

#include "Board.hpp"
#include "Board0x88.hpp"

int main() {

    Board *board = new Board0x88();
    //board->setStartingPosition();
    //board->print();
    board->setFENPosition("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
    board->print();
    board->genPseudoLegalMoves();


    return 0;
}
