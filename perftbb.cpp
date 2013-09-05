
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
using namespace std;

#include "Board.hpp"
#include "BitBoard.hpp"
#include "Move.hpp"
#include "Timers.hpp"
#include "PerftData.hpp"



//TODO checks, checkmates..
static void perft(BitBoard &board, Move *last_move, int depth, PerftData &pdata) {
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

static void pipaperft(BitBoard &board, int depth, int splitdepth, PerftData &pdata) {
    vector<Move> moves;

    if(depth == pdata.depth) {
        pdata.results[PERFT_NODES]++;
    } else {
        moves.clear();
        board.genPseudoLegalMoves(moves);

        for(Move &m : moves) {
            if(depth == splitdepth) {
                pdata.results[PERFT_NODES] = 0UL;
            }
            if(board.makeMoveIfLegal(m)) {
                pipaperft(board, depth+1, splitdepth, pdata);
                board.undoLastMove();
                if(depth == splitdepth) {
                    cout << m.toString() << " : " << pdata.results[PERFT_NODES] << endl;
                }
            }
        }
    }
}

static void runTests(vector<PerftData> &test_set) {
    BitBoard board;
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
    runTests(pos1);
    runTests(pos2);
    runTests(pos3);
    runTests(pos4);
    runTests(pos5);
    runTests(pos6);



    //MOPY
    //PerftData aaa("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603ULL, 757163, 1929, 128013, 15172, 25523, 43);
    //PerftData aaa("r3k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1", 3, 0, 0, 0, 0, 0, 0, 0);
    //PerftData aaa("1r2k2r/p1ppqNb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQk - 1 2", 2, 0, 0, 0, 0, 0, 0, 0);

    //Board0x88 bb;
    //bb.setFENPosition(aaa.fen);
    //pipaperft(bb, 0, 0, aaa);


/*
    vector<PerftData> divided;
//2. e1g1 moves =       2059 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 1 1", 2, 2059, 0, 0, 0, 0, 0, 0));
//2. e1c1 moves =       1887 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/2KR3R b kq - 1 1", 2, 1887, 0, 0, 0, 0, 0, 0));
//    2. g2h3 moves =       1970 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1P/PPPBBP1P/R3K2R b KQkq - 0 1", 2, 1970, 0, 0, 0, 0, 0, 0));
//    2. g2g3 moves =       1882 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2QPp/PPPBBP1P/R3K2R b KQkq - 0 1", 2, 1882, 0, 0, 0, 0, 0, 0));
//    2. g2g4 moves =       1843 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P1P1/2N2Q1p/PPPBBP1P/R3K2R b KQkq g3 0 1", 2, 1843, 0, 0, 0, 0, 0, 0));
//    2. b2b3 moves =       1964 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/1PN2Q1p/P1PBBPPP/R3K2R b KQkq - 0 1", 2, 1964, 0, 0, 0, 0, 0, 0));
//    2. a2a3 moves =       2186 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1", 2, 2186, 0, 0, 0, 0, 0, 0));
//    2. a2a4 moves =       2149 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1", 2, 2149, 0, 0, 0, 0, 0, 0));
//    2. d5e6 moves =       2241 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2Pnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1", 2, 2241, 0, 0, 0, 0, 0, 0));
//    2. d5d6 moves =       1991 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn1Ppnp1/4N3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1", 2, 1991, 0, 0, 0, 0, 0, 0));
//    2. c3a4 moves =       2203 ( 0.000 sec)
//    2. c3b5 moves =       2138 ( 0.000 sec)
//    2. c3d1 moves =       2040 ( 0.000 sec)
//    2. c3b1 moves =       2038 ( 0.000 sec)
//    2. e5c6 moves =       2027 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bnN1pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 1 1", 2, 2027, 0, 0, 0, 0, 0, 0));
//    2. e5d7 moves =       2124 ( 0.000 sec)
//    2. e5f7 moves =       2080 ( 0.000 sec)
//    2. e5g6 moves =       1997 ( 0.000 sec)
//    2. e5g4 moves =       1878 ( 0.000 sec)
//    2. e5d3 moves =       1803 ( 0.000 sec)
//    2. e5c4 moves =       1880 ( 0.000 sec)
//    2. f3g3 moves =       2214 ( 0.000 sec)
//    2. f3h3 moves =       2360 ( 0.000 sec)
//    2. f3e3 moves =       2174 ( 0.000 sec)
//    2. f3d3 moves =       2005 ( 0.000 sec)
//    2. f3f4 moves =       2132 ( 0.000 sec)
//    2. f3f5 moves =       2396 ( 0.000 sec)
//    2. f3f6 moves =       2111 ( 0.000 sec)
//    2. f3g4 moves =       2169 ( 0.000 sec)
//    2. f3h5 moves =       2267 ( 0.000 sec)
//    2. d2e3 moves =       2136 ( 0.000 sec)
//    2. d2f4 moves =       2000 ( 0.000 sec)
//    2. d2g5 moves =       2134 ( 0.000 sec)
//    2. d2h6 moves =       2019 ( 0.000 sec)
//    2. d2c1 moves =       1963 ( 0.000 sec)
//    2. e2d3 moves =       2050 ( 0.000 sec)
//    2. e2c4 moves =       2082 ( 0.000 sec)
//    2. e2b5 moves =       2057 ( 0.000 sec)
//    2. e2a6 moves =       1907 ( 0.000 sec)
//    2. e2f1 moves =       2060 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3KB1R b KQkq - 1 1", 2, 2060, 0, 0, 0, 0, 0, 0));
//    2. e2d1 moves =       1733 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R2BK2R b KQkq - 1 1", 2, 1733, 0, 0, 0, 0, 0, 0));
//    2. a1b1 moves =       1969 ( 0.000 sec)
//    2. a1c1 moves =       1968 ( 0.000 sec)
//    2. a1d1 moves =       1885 ( 0.000 sec)
//    2. h1g1 moves =       2013 ( 0.000 sec)
//    2. h1f1 moves =       1929 ( 0.000 sec)
//    2. e1f1 moves =       1855 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R b kq - 1 1", 2, 1855, 0, 0, 0, 0, 0, 0));
//    2. e1d1 moves =       1894 ( 0.000 sec)
    divided.push_back(PerftData("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R2K3R b kq - 1 1", 2, 1894, 0, 0, 0, 0, 0, 0));

    runTests(divided);
*/
    return 0;
}
