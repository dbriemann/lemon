
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
using namespace std;

#include "Board.hpp"
#include "Board0x88.hpp"
#include "Move.hpp"
#include "Timers.hpp"
#include "PerftData.hpp"



//TODO checks, checkmates..
static void perft(Board0x88 &board, Move *last_move, int depth, PerftData &pdata) {
    vector<Move> moves;

    if(depth == pdata.depth) {
        if(last_move->capture_type != EMPTY) {
            pdata.results[PERFT_CAPTURES]++;
        }
        if(last_move->move_type == MOVETYPE_EN_PASSENT) {
            pdata.results[PERFT_ENPASSENTS]++;
        } else if(last_move->move_type == MOVETYPE_CASTLE_LONG || last_move->move_type == MOVETYPE_CASTLE_SHORT) {
            //cout << "COLOR: " << COLORS[(board.to_move+1) % 2] << " MOVE: " << last_move->toString() << endl;
            //board.print();
            pdata.results[PERFT_CASTLES]++;
        } else if(last_move->move_type == MOVETYPE_PROMOTION_B || last_move->move_type == MOVETYPE_PROMOTION_N
                  || last_move->move_type == MOVETYPE_PROMOTION_Q || last_move->move_type == MOVETYPE_PROMOTION_R) {
            pdata.results[PERFT_PROMOTIONS]++;
        }

        pdata.results[PERFT_NODES]++;
    } else {
        moves.clear();
        board.genPseudoLegalMoves(moves);

        for(Move &m : moves) {
            if(board.makeMoveIfLegal(m)) {
                perft(board, &m, depth+1, pdata);
                board.undoLastMove();
            }
        }
    }
}

static void runTests(vector<PerftData> &test_set) {
    Board0x88 board;
    CPUTimer cpu_timer;
    double seconds = 0;

    cout << "************ PERFT ************" << endl;
    cout << "### FEN: " << test_set[0].fen << " ###" << endl;
    cout << setfill('-') << "|" << setw(125) << "|" << endl;
    cout << "|" << setw(5) << "Depth"
         << "|" << setw(15) << "Nodes[?]"
         << "|" << setw(13) << "Captures[?]"
         << "|" << setw(10) << "E.p.[?]"
         << "|" << setw(12) << "Castles[?]"
         << "|" << setw(15) << "Promotions[?]"
         << "|" << setw(11) << "Checks[?]"
         << "|" << setw(14) << "Checkmates[?]"
         << "|" << setw(10) << "Time"
         << "|" << setw(10) << "Node/sec"
         << "|" << endl;
    cout << "|" << setfill('-') << setw(125) << "|" << endl;


    for(auto &pd : test_set) {
        //set fen string
        board.setFENPosition(pd.fen);

        //run perft and take time
        cpu_timer.start();
        perft(board, NULL, 0, pd);
        seconds = cpu_timer.get_elapsed();

        //output results..
        cout << setfill(' ')
             << "|" << setw(5) << pd.depth
             << "|" << setw(12) << pd.results[PERFT_NODES] << "[" << (pd.results[PERFT_NODES] == pd.targets[PERFT_NODES] ? "+" : "-") << "]"
             << "|" << setw(10) << pd.results[PERFT_CAPTURES] << "[" << (pd.results[PERFT_CAPTURES] == pd.targets[PERFT_CAPTURES] ? "+" : "-") << "]"
             << "|" << setw(7) << pd.results[PERFT_ENPASSENTS] << "[" << (pd.results[PERFT_ENPASSENTS] == pd.targets[PERFT_ENPASSENTS] ? "+" : "-") << "]"
             << "|" << setw(9) << pd.results[PERFT_CASTLES] << "[" << (pd.results[PERFT_CASTLES] == pd.targets[PERFT_CASTLES] ? "+" : "-") << "]"
             << "|" << setw(12) << pd.results[PERFT_PROMOTIONS] << "[" << (pd.results[PERFT_PROMOTIONS] == pd.targets[PERFT_PROMOTIONS] ? "+" : "-") << "]"
             << "|" << setw(8) << pd.results[PERFT_CHECKS] << "[" << (pd.results[PERFT_CHECKS] == pd.targets[PERFT_CHECKS] ? "+" : "-") << "]"
             << "|" << setw(11) << pd.results[PERFT_CHECKMATES] << "[" << (pd.results[PERFT_CHECKMATES] == pd.targets[PERFT_CHECKMATES] ? "+" : "-") << "]"
             << "|" << setw(10) << fixed << setprecision(3) << seconds
             << "|" << setw(10) << (int)(pd.results[PERFT_NODES] / seconds)
             << "|" << endl;
    }
    cout << "|" << setfill('-') << setw(125) << "|" << endl;

    cout << "************************" << endl << endl;
}

int main() {
    //starting position //pos1
    vector<PerftData> pos1;
    pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20ULL, 0, 0, 0, 0, 0, 0));
    pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400ULL, 0, 0, 0, 0, 0, 0));
    pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902ULL, 34, 0, 0, 0, 12, 0));
    pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281ULL, 1576, 0, 0, 0, 469, 8));
    pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609ULL, 82719, 258, 0, 0, 27351, 347));
    pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324ULL, 2812008, 5248, 0, 0, 809099, 10828));
    //pos1.push_back(PerftData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 7, 3195901860ULL, 0, 0, 0, 0, 0, 0));

    //TODO ULL
    vector<PerftData> pos2;
    pos2.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 1, 48ULL, 8, 0, 2, 0, 0, 0));
    pos2.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2, 2039ULL, 351, 1, 91, 0, 3, 0));
    pos2.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3, 97862ULL, 17102, 45, 3162, 0, 993, 1));
    pos2.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603ULL, 757163, 1929, 128013, 15172, 25523, 43));
    pos2.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690ULL, 35043416, 73365, 4993637, 8392, 3309887, 30171));

    vector<PerftData> pos3;
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 1, 14ULL, 1, 0, 0, 0, 2, 0));
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 2, 191ULL, 14, 0, 0, 0, 10, 0));
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 3, 2812ULL, 209, 2, 0, 0, 267, 0));
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 4, 43238ULL, 3348, 123, 0, 0, 1680, 17));
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5, 674624ULL, 52051, 1165, 0, 0, 52950, 0));
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 6, 11030083ULL, 940350, 33325, 0, 7552, 452473, 2733));
    pos3.push_back(PerftData("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 7, 178633661ULL, 14519036, 294874, 0, 140024, 12797406, 87));

    vector<PerftData> pos4;
    pos4.push_back(PerftData("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 1, 6ULL, 0, 0, 0, 0, 0, 0));
    pos4.push_back(PerftData("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 2, 264ULL, 87, 0, 6, 48, 10, 0));
    pos4.push_back(PerftData("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3, 9467ULL, 1021, 4, 0, 120, 38, 22));
    pos4.push_back(PerftData("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, 422333ULL, 131393, 0, 7795, 60032, 15492, 5));
    pos4.push_back(PerftData("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5, 15833292ULL, 2046173, 6512, 0, 329464, 200568, 50562));
    pos4.push_back(PerftData("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, 706045033ULL, 210369132, 212, 10882006, 81102984, 26973664, 81076));

    vector<PerftData> pos5;
    pos5.push_back(PerftData("rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6", 1, 42ULL, 0, 0, 0, 0, 0, 0));
    pos5.push_back(PerftData("rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6", 2, 1352ULL, 0, 0, 0, 0, 0, 0));
    pos5.push_back(PerftData("rnbqkb1r/pp1p1ppp/2p5/4P3/2B5/8/PPP1NnPP/RNBQK2R w KQkq - 0 6", 3, 53392ULL, 0, 0, 0, 0, 0, 0));

    vector<PerftData> pos6;
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 1, 46ULL, 0, 0, 0, 0, 0, 0));
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 2, 2079ULL, 0, 0, 0, 0, 0, 0));
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3, 89890ULL, 0, 0, 0, 0, 0, 0));
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3894594ULL, 0, 0, 0, 0, 0, 0));
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 5, 164075551ULL, 0, 0, 0, 0, 0, 0));
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 6, 6923051137ULL, 0, 0, 0, 0, 0, 0));
    pos6.push_back(PerftData("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 7, 287188994746ULL, 0, 0, 0, 0, 0, 0));


    //run all tests..
    //runTests(pos1);
    runTests(pos2);
    //runTests(pos3);
    //runTests(pos4);
    //runTests(pos5);
    //runTests(pos6);

    return 0;
}
