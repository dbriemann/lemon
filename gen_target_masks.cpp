
#include <iostream>
using namespace std;

#include "BitBoard.hpp"
#include "lemon.hpp"

void genKnightsTargets() {
    cout << "static const U64 KNIGHT_TARGET_BBS[64] = {";

    for(int i = 0; i < 64; i++) {
        if(i >= 1) {
            cout << ", ";
        }
        if(i % 8 == 0) {
            cout << endl << "    ";
        }

        U64 pos = iBitMask(i);
        U64 targets = C64(0);

        targets |= _SHIFT_ENE(pos);
        targets |= _SHIFT_ESE(pos);
        targets |= _SHIFT_NNE(pos);
        targets |= _SHIFT_NNW(pos);
        targets |= _SHIFT_WNW(pos);
        targets |= _SHIFT_WSW(pos);
        targets |= _SHIFT_SSE(pos);
        targets |= _SHIFT_SSW(pos);


        //reverse invalid targets
        //files
        if(pos & FILE_A) {
            targets &= ~_SHIFT_NNW(pos);
            targets &= ~_SHIFT_WNW(pos);
            targets &= ~_SHIFT_WSW(pos);
            targets &= ~_SHIFT_SSW(pos);
        } else if(pos & FILE_B) {
            targets &= ~_SHIFT_WNW(pos);
            targets &= ~_SHIFT_WSW(pos);
        } else if(pos & FILE_G) {
            targets &= ~_SHIFT_ENE(pos);
            targets &= ~_SHIFT_ESE(pos);
        } else if(pos & FILE_H) {
            targets &= ~_SHIFT_NNE(pos);
            targets &= ~_SHIFT_ENE(pos);
            targets &= ~_SHIFT_ESE(pos);
            targets &= ~_SHIFT_SSE(pos);
        }

        //ranks
        if(pos & RANK_1) {
            targets &= ~_SHIFT_WSW(pos);
            targets &= ~_SHIFT_SSW(pos);
            targets &= ~_SHIFT_SSE(pos);
            targets &= ~_SHIFT_ESE(pos);
        } else if(pos & RANK_2) {
            targets &= ~_SHIFT_SSW(pos);
            targets &= ~_SHIFT_SSE(pos);
        } else if(pos & RANK_7) {
            targets &= ~_SHIFT_NNW(pos);
            targets &= ~_SHIFT_NNE(pos);
        } else if(pos & RANK_8) {
            targets &= ~_SHIFT_WNW(pos);
            targets &= ~_SHIFT_NNW(pos);
            targets &= ~_SHIFT_NNE(pos);
            targets &= ~_SHIFT_ENE(pos);
        }

        cout << "0x" << hex << targets << "ULL";

    }
    cout << endl << "};" << endl;
}



int main() {

    genKnightsTargets();

    return 0;
}
