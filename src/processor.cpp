#include "processor.h"


Processor::Processor():
    idle_(LinuxParser::IdleJiffie()),
    nonIdle_(LinuxParser::ActiveJiffies()),
    totalIdle_(idle_ + nonIdle_) {}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long idle = LinuxParser::IdleJiffies();
    long nonIdle = LinuxParser::ActiveJiffies();
    long totalIdle = idle + nonIdle;

    long deltaIdle = idle - idle_;
    long deltaTotal = totalIdle - totalIdle_;

    idle_ = idle;
    nonIdle = nonIdle; 
    totalIdle = totalIdle; 

    return (deltaTotal - deltaIdle)/deltaTotal;
}