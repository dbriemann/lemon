
#include <iostream>
#include <vector>
using namespace std;

#include "Board.hpp"
#include "Board0x88.hpp"
#include "Move.hpp"

int main() {

    Board *board = new Board0x88();
    board->setStartingPosition();
    //board->print();

    //board->setFENPosition("r1b2bnr/1pqkpPpp/p1B5/2pp4/8/5N2/PPPP1PPP/RNBQK2R b KQ - 0 7");
    //board->print();
    //((Board0x88 *)board)->removePiece(0, ((Board0x88 *)board)->pieces[WHITE]);
    //board->print();


    while(true) {
        uint8_t last_ptype = PAWN;
        board->print();
        vector<Move> moves = board->genPseudoLegalMoves();

        int i = 0;
        for(Move m : moves) {
            if((moveGetPType(m) & MASK_PIECE) != last_ptype) {
                cout << endl;
                last_ptype = moveGetPType(m) & MASK_PIECE;
            }
            cout << i << ": " << moveToString(m) << " ";
            i++;
        }
        cout << endl;

        cout << "Enter Move number: " << endl;
        int mnum = 0;
        cin >> mnum;
        cout << endl;

        board->makeMove(moves[mnum]);
    }


    return 0;
}
