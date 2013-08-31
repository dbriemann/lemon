
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

    //Board *board = new Board0x88();
    Board0x88 board;
    //board.setStartingPosition();
    //string fen_old = board->getFENCode();
    //board.print();

    //board->setFENPosition("r1b2bnr/1pqkpPpp/p1B5/2pp4/8/5N2/PPPP1PPP/RNBQK2R b KQ - 0 7");
    //board->print();
    //((Board0x88 *)board)->removePiece(0, ((Board0x88 *)board)->pieces[WHITE]);
    //board->print();

    vector<Move> moves;

    for(int x = 0; x < 100; x++) {
        board.setStartingPosition();
        board.genPseudoLegalMoves(moves);

        while(moves.size() > 0) {
        //for(int i = 0; i < 200; i++) {
        //while(board->makeMoveIfLegal(moves[mnum])) {

            //if(!board->makeMoveIfLegal(moves[mnum])) {
            //    break;
            //}
            int mnum = rand() % moves.size();
            board.makeMove(moves[mnum]);
            board.print();
            //board.makeMoveIfLegal(moves[mnum]);
            moves.clear();
            board.genPseudoLegalMoves(moves);
        }
    }

    return 0;
}
