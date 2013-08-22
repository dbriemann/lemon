#ifndef UTILITY_FUNCTIONS_HPP
#define UTILITY_FUNCTIONS_HPP

#include <sstream>
#include <string>
using namespace std;

static string intToString(int number) {
   stringstream ss;     //create a stringstream
   ss << number;        //add number to the stream
   return ss.str();     //return a string with the contents of the stream
}

#endif // UTILITY_FUNCTIONS_HPP
