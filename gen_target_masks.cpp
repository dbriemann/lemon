
#include <iostream>
#include <bitset>
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

        cout << "0x" << hex << uppercase << targets << "ULL";

    }
    cout << endl << "};" << endl;
}

void genKingTargets() {
    cout << "static const U64 KING_TARGET_BBS[64] = {";
    for(int i = 0; i < 64; i++) {
        if(i >= 1) {
            cout << ", ";
        }
        if(i % 8 == 0) {
            cout << endl << "    ";
        }
        U64 pos = iBitMask(i);
        U64 targets = C64(0);

        targets |= (_SHIFT_N(pos) & ~RANK_1);
        targets |= (_SHIFT_S(pos) & ~RANK_8);
        targets |= (_SHIFT_W(pos) & ~FILE_H);
        targets |= (_SHIFT_E(pos) & ~FILE_A);
        targets |= (_SHIFT_NW(pos) & ~FILE_H & ~RANK_1);
        targets |= (_SHIFT_NE(pos) & ~FILE_A & ~RANK_1);
        targets |= (_SHIFT_SW(pos) & ~FILE_H & ~RANK_8);
        targets |= (_SHIFT_SE(pos) & ~FILE_A & ~RANK_8);

        cout << "0x" << hex << uppercase << targets << "ULL";
        //PRINTBB(targets, "KING TARGETS");
    }

    cout << endl << "};" << endl;
}

void printRank(U64 rank) {
    bitset<8> p((U8)reverse(rank));
    cout << p;
}

void genBlockedFirstRankMasks() {
    cout << "static const U64 RANK_ATTACK_BBS[8][64] = {" << endl;
    for(int file = 0; file < 8; file++) {
        if(file >= 1) {
            cout << "," << endl;
        }
        cout << "{";
        U64 piece_at = iBitMask(file);
        //cout << "------------------------------------------*" << endl;
        //printRank(piece_at);

        for(U64 occ = 0; occ < 64; occ++) {
            if(occ >= 1) {
                cout << ", ";
            }
            U64 result = 0;
            U64 wanderer = piece_at;
            U64 occu = (occ << 1) | 0x81;
            //printRank(occu);

            //move east
            do {
                wanderer = _SHIFT_E(wanderer) & ~FILE_A;
                result |= wanderer;
            } while(((wanderer & occu) == 0) && (wanderer != 0));

            wanderer = piece_at;
            //move west
            do {
                wanderer = _SHIFT_W(wanderer) & ~FILE_H;
                result |= wanderer;
            } while(((wanderer & occu) == 0) && (wanderer != 0));

            cout << "0x" << hex << uppercase << result;

            //printRank(result);
        }
        cout << "}";

        //cout << "----------------------------------------------" << endl;
    }
    cout << endl << "};" << endl;
}

void genBlockedFirstFileMasks() {
    cout << "static const U64 FILE_ATTACK_BBS[8][64] = {" << endl;
    for(int rank = 0; rank < 8; rank++) {
        if(rank >= 1) {
            cout << "," << endl;
        }
        cout << "{";
        U64 piece_at = iBitMask(rank*8);
        //cout << "--------------*****************+----------------------------*" << endl;
        //printRank(piece_at);

        for(U8 occ = 0; occ < 64; occ++) {
            //cout << "--------------*****************+----------------------------*" << endl;
            //PRINTBB(piece_at, "PIECE");
            if(occ >= 1) {
                cout << ", ";
            }
            U64 result = 0;
            U64 wanderer = piece_at;
            U64 innerocc = (U64)(reverse(occ) >> 2);

            U64 occu = 0;
            while(innerocc) {
                U64 bit = bitscanfwd(innerocc);

                occu |= iBitMask((bit+1) * 8);

                innerocc &= ~iBitMask(bit);
            }
            occu |= iBitMask(0) | iBitMask(56);
            //PRINTBB(occu, "OCCU");


            //move north
            do {
                wanderer = _SHIFT_N(wanderer) & ~RANK_1;
                result |= wanderer;
            } while(((wanderer & occu) == 0) && (wanderer != 0));

            wanderer = piece_at;
            //move south
            do {
                wanderer = _SHIFT_S(wanderer) & ~RANK_8;
                result |= wanderer;
            } while(((wanderer & occu) == 0) && (wanderer != 0));

            cout << "0x" << hex << uppercase << result;

            //PRINTBB(result, "RESULT");
        }
        cout << "}";

        //cout << "----------------------------------------------" << endl;
    }
    cout << endl << "};" << endl;
}

//a1 to h8
void genBlockedDiagAttack() {
    cout << "static const U64 DIAG_ATTACK_BBS[8][64] = {" << endl;
    for(int file = 0; file < 8; file++) {
        if(file >= 1) {
            cout << "," << endl;
        }
        cout << "{";
        U64 piece_at = iBitMask(file*8 + file);

        for(U8 occ = 0; occ < 64; occ++) {
            if(occ >= 1) {
                cout << ", ";
            }
            //cout << "--------------*****************+----------------------------*" << endl;
            U64 result = 0;
            U64 wanderer = piece_at;
            //PRINTBB(piece_at, "PIECE");
            U64 occu = 0;
            U64 innerocc = (occ << 1);
            while(innerocc) {
                U32 bit = bitscanfwd(innerocc);

                occu |= iBitMask(bit * 8 + bit);

                innerocc &= ~iBitMask(bit);
            }
            occu |= iBitMask(0) | iBitMask(63);
            //PRINTBB(occu, "OCC");

            //move NE
            do {
                wanderer = _SHIFT_NE(wanderer) & ~RANK_1 & ~FILE_A;
                result |= wanderer;
            } while(((wanderer & occu) == 0) && (wanderer != 0));

            wanderer = piece_at;
            //move SW
            do {
                wanderer = _SHIFT_SW(wanderer) & ~RANK_8 & ~FILE_H;
                result |= wanderer;
            } while(((wanderer & occu) == 0) && (wanderer != 0));

            //PRINTBB(result, "RESULT");
            cout << "0x" << hex << uppercase << result;
        }
        cout << "}";
    }
    cout << endl << "};" << endl;
}


int main() {

    //genKnightsTargets();
    //genKingTargets();
    //genBlockedFirstRankMasks();
    //genBlockedFirstFileMasks();

    //TODO diagonal masks...

    genBlockedDiagAttack();

    return 0;
}
