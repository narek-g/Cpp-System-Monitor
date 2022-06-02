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

Process::Process(const int id): pid_(id){
    // username_ = LinuxParser::User(pid_);
    // command_ = LinuxParser::Command(pid_);
    timeHerze_ = sysconf(_SC_CLK_TCK);
    string line; 
    std::ifstream stream(LinuxParser::kProcDirectory + to_string(id) + LinuxParser::kStatFilename);
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::isteam_iterator<string> beg(linestream), end;
    vector<string> cpuTimes(beg, end);
    utime_ = std::stof(cpuTimes[13]);
    stime_ = std::stof(cpuTimes[14]);
    cutime_ = std::stof(cpuTimes[15]);
    cstime_ = std::stof(cpuTimes[16]);
    startTime_ = std::stof(cpuTimes[21]);
    cpuUtilization_ = CpuUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { 
    return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    float upTime = LinuxParser::UpTime();
    float totalTime = utime_ + stime_ + cutime_ + cstime_;
    float cpuUtilization = (totalTime/timeHerze_)/(upTime - (startTime/timeHerze_));
    return cpuUtilization;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    long seconds = LinuxParser::UpTime() - (startTime_/timeHerze_)
 }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    // return this->cpuUtilization_ > a.cpuUtilization_;
    return a.cpuUtilization_ < this->cpuUtilization_;
}