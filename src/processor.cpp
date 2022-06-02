#include <string> 
#include<sstream>
#include <vector>
#include <numeric>
#include "processor.h"
#include "linux_parser.h"


// Processor::Processor():
//     idle_(LinuxParser::IdleJiffies()),
//     nonIdle_(LinuxParser::ActiveJiffies()),
//     totalIdle_(idle_ + nonIdle_) {}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
   double time;
   std::string line, key;
   std::vector<double> cpuTimes;
   std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
   if(stream.is_open()){
       while(std::getline(stream, line)){
           std::istringstream linestream(line);
           while(linestream >> key){
               if(key == "cpu"){
                   while(linestream >> time) {cpuTimes.push_back(time);}
                }
            }
        }
    }
    if(cpuTimes.size() < 4) {return false;}

    double idle = cpuTimes[3];
    double totalIdle = std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0);

    float idleDelta = idle - idle_;
    float totalDelta = totalIdle - totalIdle_;
    float utilization = 1 - (idleDelta/totalDelta);

    idle_ = idle; 
    totalIdle_ = totalIdle; 

    return(utilization);
}