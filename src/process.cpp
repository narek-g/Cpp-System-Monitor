#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int id): id_(id){
    username_ = LinuxParser::User(id_);
    command_ = LinuxParser::Command(id_);
}

// TODO: Return this process's ID
int Process::Pid() { 
    return id_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    int upTime = LinuxParser::UpTime();
    int activeTime = LinuxParser::ActiveJiffies(id_);
    int processTime = LinuxParser::UpTime(id_);
    
    int timeDiff = upTime - processTime;

    cpuUtilization_ = (1.0*activeTime)/timeDiff;
    return cpuUtilization_; 
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(id_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return username_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(id_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return this->cpuUtilization_ > a.cpuUtilization_;
}