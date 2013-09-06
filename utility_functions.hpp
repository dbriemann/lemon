#ifndef UTILITY_FUNCTIONS_HPP
#define UTILITY_FUNCTIONS_HPP

#include <sstream>
#include <string>
#include <cstdint>
#include <bitset>
using namespace std;

#include "lemon.hpp"


string intToString(int number) {
   stringstream ss;     //create a stringstream
   ss << number;        //add number to the stream
   return ss.str();     //return a string with the contents of the stream
}

void ERROR(const string msg) {
    cerr << "ERROR: " << msg << endl;
}

U8 reverse(U8 b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void PRINTBB(U64 bb, string title) {
    cerr << "-----------------" << endl;
    cerr << title << endl << endl;
    U64 mask = 0xFF00000000000000ULL;
    for(int shout = 56; shout >=0; shout -= 8) {
        U8 rank = (bb & mask) >> shout;
        bitset<8> bs(reverse(rank));
        cerr << bs << endl;
        mask >>= 8;
    }
    cerr << "-----------------" << endl;
}

#endif // UTILITY_FUNCTIONS_HPP
