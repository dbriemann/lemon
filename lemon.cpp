
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
    board->setFENPosition("rnbqkbnr/pp2pppp/4P3/2pp4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 3");
    board->print();
    list<Move> moves = board->genPseudoLegalMoves();

    for(Move m : moves) {
        cout << moveToString(m) << ", ";
    }
    cout << endl;


    return 0;
}
