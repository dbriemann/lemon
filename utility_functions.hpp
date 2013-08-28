#ifndef UTILITY_FUNCTIONS_HPP
#define UTILITY_FUNCTIONS_HPP

#include <sstream>
#include <string>
#include <cstdint>
using namespace std;

static string intToString(int number) {
   stringstream ss;     //create a stringstream
   ss << number;        //add number to the stream
   return ss.str();     //return a string with the contents of the stream
}

static void ERROR(const string msg) {
    cerr << "ERROR: " << msg << endl;
}



#endif // UTILITY_FUNCTIONS_HPP
