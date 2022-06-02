#include <string>
#include <sstream>

#include "format.h"

using std::string;


string NumToString(int number){
    // std::stringstream numstr; 

    // if(number < 10){ numstr << "0";}
    // numstr << number; 
    // return numstr.str(); 
    string timeStr = std::to_string(number);
    return(string(2 - timeStr.length(), '0') + timeStr);
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long hour, minute, remaining; 
    hour = seconds/3600; 
    remaining = seconds%3600;
    minute = (remaining)/60; 
    remaining = minute%60;

    return(NumToString(hour) + ":" + NumToString(minute) + ":" + NumToString(remaining));
 }