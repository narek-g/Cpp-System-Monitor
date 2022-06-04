#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


template <typename Type> Type getValues( const string &filename, const string &key){
  Type value; 
  string line, streamKey;
  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      // std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> streamKey >> value){
        if(streamKey == key){return value;}
      }
    } // close while getline
  } // close filestrea.is_open
  stream.close();
  return value; // empty value if not found 
} // close template class


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal = getValues<int>(LinuxParser::kMeminfoFilename, "MemTotal:");
  float memFree  = getValues<int>(LinuxParser::kMeminfoFilename, "MemFree:");
  return (memTotal - memFree)/memTotal; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  long upTime = 0; 
  string line; 
  std::ifstream stream(LinuxParser::kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
  return upTime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpuTimes = LinuxParser::CpuUtilization();
  long totalTime = 0;
  for(unsigned int i = 0; i < cpuTimes.size(); i++){
    if(i == LinuxParser::CPUStates::kGuestNice_|| i == LinuxParser::CPUStates::kGuest_){
      totalTime -= stol(cpuTimes[i]);
    } else {
      totalTime += stol(cpuTimes[i]);
    }
  }
  return totalTime;
}

// Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  std::getline(stream, line);
  std::istringstream linestream(line);
  std::istream_iterator<string> beg(linestream), end;
  vector<string> cpuTimes(beg, end);
  float utime = stof(cpuTimes[13]);
  float stime = stof(cpuTimes[14]);
  float cutime = stof(cpuTimes[15]);
  float cstime = stof(cpuTimes[16]);
  return(utime + stime + cutime + cstime); 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return(LinuxParser::Jiffies() - LinuxParser::IdleJiffies());
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpuTimes = LinuxParser::CpuUtilization();
  return(stol(cpuTimes[LinuxParser::CPUStates::kIdle_]) + stol(cpuTimes[LinuxParser::CPUStates::kIOwait_]));
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuTimes;
  string streamKey, line, value; 
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> streamKey; 
      if(streamKey == "cpu"){
        while(linestream >> value) {
          cpuTimes.push_back(value);
        }
      }
    }
  }
  filestream.close();
  return(cpuTimes);
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  return(getValues<int>(LinuxParser::kStatFilename, LinuxParser::filterProcesses));
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  return(getValues<int>(LinuxParser::kStatFilename, LinuxParser::filterRunningProcesses));
}

// Read and return the command associated with a process
// REMOVEed: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string command = "No Command";
  string line; 
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  return command;
 }

// Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  long ram = getValues<long>(to_string(pid) + LinuxParser::kStatusFilename, "VmSize:");
  return(to_string(ram/1024));
}

// Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
int LinuxParser::Uid(int pid) { 
  return(getValues<int>(to_string(pid) + LinuxParser::kStatusFilename, "Uid:"));
}

// Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { 
  int userID = LinuxParser::Uid(pid);
  string username = "User"; 
  string line, xVal;
  int streamID;
  std::ifstream filestream(LinuxParser::kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line); 
      while(linestream >> username >> xVal >> streamID){
        if(streamID == userID) {return username;}
      }
    }
  }
  return username; 
 }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
// long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }