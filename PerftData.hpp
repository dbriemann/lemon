#ifndef PERFTDATA_HPP
#define PERFTDATA_HPP

#include <cstdint>
#include <string>
using namespace std;

static const int PERFT_ENTRIES = 7;

static const int PERFT_NODES = 0;
static const int PERFT_CAPTURES = 1;
static const int PERFT_ENPASSENTS = 2;
static const int PERFT_CASTLES = 3;
static const int PERFT_PROMOTIONS = 4;
static const int PERFT_CHECKS = 5;
static const int PERFT_CHECKMATES = 6;

static const string PERFT[PERFT_ENTRIES] = {"Nodes", "Captures", "E.p.", "Castles", "Promotions", "Checks", "Checkmates"};

struct PerftData {
    string fen;
    int depth;
    uint64_t targets[PERFT_ENTRIES];
    uint64_t results[PERFT_ENTRIES];

    PerftData(string fen, int depth, uint64_t nodes, uint64_t captures, uint64_t en_passents, uint64_t castles, uint64_t promotions, uint64_t checks, uint64_t checkmates) {
        this->fen = fen;
        this->depth = depth;
        targets[PERFT_NODES] = nodes;
        targets[PERFT_CAPTURES] = captures;
        targets[PERFT_ENPASSENTS] = en_passents;
        targets[PERFT_CASTLES] = castles;
        targets[PERFT_PROMOTIONS] = promotions;
        targets[PERFT_CHECKS] = checks;
        targets[PERFT_CHECKMATES] = checkmates;

        for(int i = 0; i < PERFT_ENTRIES; i++) {
            results[i] = 0;
        }
    }

    //friend ostream& operator<< (ostream &out, PerftData &pd);
};

#endif // PERFTDATA_HPP
