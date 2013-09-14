
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
static double total_time;
static double total_nodes;
static vector<MoveList> move_lists;
static vector<BitBoard> boards;

U64 simplePerft(int depth, int maxdepth) {
    U64 nodes = 0;

    if(depth == maxdepth) return 1;

    BitBoard &last_board = boards[depth];
    BitBoard &cur_board = boards[depth+1];
    MoveList &mlist = move_lists[depth+1];
    cur_board = last_board; //copy board
    mlist.reset();
    cur_board.genPseudoLegalMoves(mlist);

    for(int i = 0; i < mlist.size; i++) {
        if(cur_board.makeLightMove(mlist.moves[i])) {
            nodes += simplePerft(depth + 1, maxdepth);
        }
        cur_board = last_board; //undo
    }

    return nodes;
}

void runTests(string fen, int maxdepth, U64 compare_results[]) {
    boards[0].setFENPosition(fen);

    cout << "******************************************************************************************" << endl;
    cout << "FEN: " << fen << endl << endl;

    for(int d = 1; d <= maxdepth; d++) {
        timer.start();
        U64 result = simplePerft(0, d);
        double t = timer.get_elapsed();
        total_time += t;
        total_nodes += result;
        cout << "Depth: " << d << " Nodes: " << result << "   ---> " << ((result == compare_results[d]) ? "^PASS^" : "_FAIL_") << endl;
        cout << "        " << "Time: " << t << " sec. | n/sec:" << fixed << (double)result/t << endl;
    }

    cout << "******************************************************************************************" << endl;
}

int main() {
    int max_depth_used = 8;
    for(int i = 0; i < max_depth_used; i++) {
        move_lists.push_back(MoveList());
        boards.push_back(BitBoard());
    }

    string fen1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    U64 set1[7] = {0ULL, 20ULL, 400ULL, 8902ULL, 197281ULL, 4865609ULL, 119060324ULL};
    runTests(fen1, 6, set1);

    string fen2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    U64 set2[6] = {0ULL, 48ULL, 2039ULL, 97862ULL, 4085603ULL, 193690690ULL};
    runTests(fen2, 5, set2);

    string fen3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    U64 set3[8] = {0ULL, 14ULL, 191ULL, 2812ULL, 43238ULL, 674624ULL, 11030083ULL};//, 178633661ULL};
    runTests(fen3, 6, set3);

    string fen4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    U64 set4[7] = {0ULL, 6ULL, 264ULL, 9467ULL, 422333ULL, 15833292ULL};//, 706045033ULL};
    runTests(fen4, 5, set4);

    string fen5 = "rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6";
    U64 set5[4] = {0ULL, 42ULL, 1352ULL, 53392ULL};
    runTests(fen5, 3, set5);

    string fen6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    U64 set6[8] = {0ULL, 46ULL, 2079ULL, 89890ULL, 3894594ULL};//, 164075551ULL};//, 6923051137ULL};//, 287188994746ULL};
    runTests(fen6, 4, set6);


    cout << "TOTAL TIME  : " << total_time << " sec." << endl;
    cout << "TOTAL NODES : " << total_nodes << endl;
    cout << "NODES/SEC   : " << (double)total_time / (double)total_nodes << endl;

    return 0;
}








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
/*
    int maxdepth = 5;


    for(int d = 1; d <= maxdepth; d++) {
        timer.start();
        U64 p = simplePerft(board, 0, d);
        double t = timer.get_elapsed();
        cout << "depth=" << d << " | perft=" << p << " | time=" << fixed << t << " sec. | n_sec=" << (double)p/t << endl;


        //split run
        MoveList mlist;
        board.genPseudoLegalMoves(mlist);
        acopy = board;

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

    }
*/
