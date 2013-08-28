
#include <iostream>
#include <vector>
#include <cstdio>
#include <ctime>
using namespace std;

#include "Board.hpp"
#include "Board0x88.hpp"
#include "Move.hpp"

int main() {
    srand(time(0));

    Board *board = new Board0x88();
    board->setStartingPosition();
    string fen_old = board->getFENCode();
    board->print();

    //board->setFENPosition("r1b2bnr/1pqkpPpp/p1B5/2pp4/8/5N2/PPPP1PPP/RNBQK2R b KQ - 0 7");
    //board->print();
    //((Board0x88 *)board)->removePiece(0, ((Board0x88 *)board)->pieces[WHITE]);
    //board->print();

    vector<Move> moves = board->genPseudoLegalMoves();

    while(moves.size() > 0) {
    //for(int i = 0; i < 100; i++) {
        int mnum = rand() % moves.size();

        //cout << "MOVE TAKEN: " << moveToString(moves[mnum]) << endl;

        board->makeMove(moves[mnum]);
        moves = board->genPseudoLegalMoves();
        //if(moves.empty()) {break;}
    }

    cout << endl << endl << endl << endl << endl;

    while(board->undoLastMove()) {
        board->print();
    }

    //board->print();
    string fen_new = board->getFENCode();

    if(fen_old == fen_new) {
        cout << "HIT" << endl;
    } else {
        cout << "FAIL" << endl;
    }


    //board->print();


    return 0;
}
