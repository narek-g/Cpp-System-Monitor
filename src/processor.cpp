#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long idle = LinuxParser::IdleJiffies();
    long nonIdle = LinuxParser::ActiveJiffies();
    long totalIdle = idle + nonIdle;

    idle_(LinuxParser::IdleJiffies);
    nonIdle_(LinuxParser::ActiveJiffies)
    totalIdle_(idle_ + nonIdle_);

    long deltaIdle = idle - idle_;
    long deltaTotal = totalIdle - totalIdle_;

    return (deltaTotal - deltaIdle)/deltaTotal;
}