
#include <iostream>
#include <vector>
#include <cstdio>
#include <ctime>
using namespace std;

#include "BitBoard.hpp"



int main() {
    srand(time(0));

    BitBoard bboard;
    bboard.setStartingPosition();
    cout << "WHITE: " << bboard.pieces_by_color[WHITE] << endl;
    cout << "BLACK: " << bboard.pieces_by_color[BLACK] << endl;
    bboard.print();


    return 0;
}
