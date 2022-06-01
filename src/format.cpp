#include <string>
#include <sstream>

#include "format.h"

using std::string;


string Format::NumToString(long number){
    std::stringstream numstr; 

    if(number < 10){ numstr << "0";}
    numstr << number; 
    return numstr.str(); 
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long hour, minute, second; 
    hour = seconds/3600; 
    long remaining = seconds%3600;
    minute = (remainding)/60; 
    remaining = minute%60;

    std::stringstream numstr; 
    numstr << NumToString(hour) << ":" << NumToString(minute) << ":" << NumToString(remaining);
    return numstr.str();
 }