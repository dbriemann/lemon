
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

    if(depth == maxdepth) return 1;

    BitBoard current_board(board);
    current_board.genPseudoLegalMoves(mlist);
    //cout << "Moves: " << mlist.size << endl;

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
    BitBoard acopy;
    BitBoard bcopy;
    BitBoard ccopy;
    //board.setStartingPosition();
    board.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    //board.setFENPosition("r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");
    //board.setFENPosition("r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q2/PPPBBPpP/R3K2R w KQkq - 0 2");
    //board.setFENPosition("r3k2r/p1ppqpb1/1n2pnN1/3P4/1p2P3/2N2Q1p/PPPBbPPP/R3K2R w KQkq - 0 2");
    //board.setFENPosition("r3k2r/p1ppqpb1/1n2pnN1/1N1P4/1p2P3/5Q1p/PPPBbPPP/R3K2R b KQkq - 1 2");
    //board.setFENPosition("2kr3r/p1ppqpb1/1n2pnN1/1N1P4/1p2P3/5Q1p/PPPBbPPP/R3K2R w KQ - 2 3");
    //board.setFENPosition("2kr3r/p1ppqpb1/1n2pnN1/1N1P4/1p2P3/5Q1P/PPPBbP1P/R3K2R b KQ - 0 3");

    //board.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R b kq - 1 1");
    //board.setFENPosition("r4rk1/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R w - - 2 2");
    //board.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 1 1");
    //board.setFENPosition("r3k2r/p2pqpb1/bn2pnp1/1BpPN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R w KQkq c6 0 2");

    //board.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 1 1");
    //board.setFENPosition("r3k3/p1ppqpb1/bn2pnp1/3PN3/1p2P2r/2N2Q1p/PPPBBPPP/R4RK1 w q - 2 2");
    //board.setFENPosition("r3k2r/p1ppqpb1/bn2p1p1/3PN3/1p2n3/2N2Q1p/PPPBBPPP/R4RK1 w kq - 0 2");
    //board.setFENPosition("r3k2r/p1ppqNb1/bn2p1p1/3P4/1p2n3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 0 2");

    //board.setFENPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R b kq - 1 1");
    //board.setFENPosition("r3k2r/p1ppqpb1/1n2pnp1/3PN3/1p2P3/2N2Q1p/PPPBbPPP/R4K1R w kq - 0 2");
/*
    MoveList mlist;
    acopy = board;
    acopy.genPseudoLegalMoves(mlist);
    for(int i = 0; i < mlist.size; i++) {
        if(acopy.makeLightMove(mlist.moves[i])) {
            cout << moveToStr(mlist.moves[i]) << " --> " << acopy.getFENCode() << endl;

        }
        acopy = board;
    }
    cout << endl;
*/

    int maxdepth = 6;


    for(int d = 1; d <= maxdepth; d++) {
        timer.start();
        int p = simplePerft(board, 0, d);
        double t = timer.get_elapsed();
        cout << "depth=" << d << " | perft=" << p << " | time=" << fixed << t << " sec. | n_sec=" << (double)p/t << endl;


        //split run
        MoveList mlist;
        board.genPseudoLegalMoves(mlist);
        acopy = board;
/*
        if(d > 1) {
            for(int i = 0; i < mlist.size; i++) {
                cout << moveToStr(mlist.moves[i]) << "   ";

                if(acopy.makeLightMove(mlist.moves[i])) {
                    int sp = simplePerft(acopy, 0, d-1);
                    cout << "perft= " << sp << endl;//<< " --> " << copyboard.getFENCode() << endl;

                    bcopy = acopy;
                    MoveList ml;
                    bcopy.genPseudoLegalMoves(ml);
                    if(i == 35) {
                        for(int m = 0; m < ml.size; m++) {
                            if(bcopy.makeLightMove(ml.moves[m])) {
                                cout << "    " << moveToStr(ml.moves[m]) << "   ";
                                int s = simplePerft(bcopy, 0, d-2);
                                cout << "perft= " << s << endl;//<< " --> " << copyboard.getFENCode() << endl;
                                //bcopy.print();

                                if(m == 43) {
                                    ccopy = bcopy;
                                    ccopy.print();
                                    MoveList ml2;
                                    ccopy.genPseudoLegalMoves(ml2);
                                    for(int z = 0; z < ml2.size; z++) {
                                        if(ccopy.makeLightMove(ml2.moves[z])) {
                                            cout << "        " << moveToStr(ml2.moves[z]) << "   ";
                                            int abc = simplePerft(ccopy, 0, d-3);
                                            cout << "perft= " << abc << endl;//<< " --> " << copyboard.getFENCode() << endl;
    //                                        if(ccopy.makeLightMove(ml2.moves[z])) {
    //                                            cout << "        " << moveToStr(ml2.moves[z]) << endl;
    //                                        }
                                        }
                                        ccopy = bcopy;
                                    }
                                }
                            }
                            bcopy = acopy;
                        }
                    }


                } else {
                    cout << "invalid" << endl;
                }
                acopy = board;
            }
        }
*/
    }


    return 0;
}
