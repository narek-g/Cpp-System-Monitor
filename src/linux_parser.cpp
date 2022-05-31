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


template <typename Type> Type getValues( const string &path, const string &key){
  Type value; 
  std::ifstream filestream(path);
  if (filestream.is_open()){
    string line; 

    while (std::getline(filestream, line)) {
      // std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string streamKey; 
      linestream >> streamKey; 
      if(streamKey == key){
        linestream >> value; 
        return value; 
      } // close if key found 
    } // close while getline
  } // close filestrea.is_open
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
  string os, kernel, version;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal, memFree; 
  const string path(kProcDirectory + kMeminfoFilename);
  memTotal = getValues<float> (path, "MemTotal:");
  memFree  = getValues<float> (path, "MemFree:");
  return (memTotal - memFree)/memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptimeValue; 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()){
    filestream >> uptimeValue;
  }
  return uptimeValue; 
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpuTimes = CpuUtilization();
  long totalTime = 0; 
  for(unsigned int i = 0;i < cpuTimes.size(); i++) {
    if(i == CPUStates::kGuest_ || CPUStates::kGuestNice_){
      totalTime -= stol(cpuTimes[i]);
    } else {
      totalTime += stol(cpuTimes[i]);
    }
  } 
  return totalTime; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long totalTime = 0; 
  string xVal; 
  std::stringstream path; 
  path << kProcDirectory << pid << kStatFilename; 
  std::ifstream filestream(path.str());
  if(filestream.is_open()){
    const int position = 13; 
    for(int i = 0; i < position; i++){
      if(!(filestream >> xVal)){
        return 1000; 
      }
    }
    long clock1, clock2, clock3, clock4; 
    if(filestream >> clock1 >> clock2 >> clock3 >> clock4){
      return(clock1 + clock2 + clock3 + clock4)/sysconf(_SC_CLK_TCK);
    }
  }
 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpuJiffies = CpuUtilization();
  return stol(cpuJiffies[kUser_] + cpuJiffies[kNice_] + cpuJiffies[kSystem_] + 
  cpuJiffies[kIRQ_] + cpuJiffies[kSoftIRQ_] + cpuJiffies[kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpuJiffies = CpuUtilization();
  return stol(cpuJiffies[kIdle_] + cpuJiffies[kIOwait_]);
 }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuTimes; 
  string cpuValue, line, key; 

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key; 
      if(key == "cpu"){
        while(linestream >> cpuValue){
          cpuTimes.push_back(cpuValue);
        }
      }
    }
  }
  return cpuTimes; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int totalProc; 
  string path(kProcDirectory + kStatFilename);
  totalProc = getValues<int> (path, "processes");
  return totalProc; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int runProc; 
  string path(kProcDirectory + kStatFilename);
  runProc = getValues<int> (path, "procs_running");
  return runProc; 
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command = "No command";

  std::stringstream path;
  path << kProcDirectory << pid << kCmdlineFilename;

  std::ifstream filestream(path.str());
    if (filestream.is_open()) {
    
    string line;
      if (std::getline(filestream, line)) {
        //command = line;
        return line; 
      }
  }
  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::stringstream path; 
  int Ram;
  std::stringstream returnRam; 
  path << kProcDirectory << pid << kStatusFilename;
  Ram = getValues<int> (path.str(), "VmSize:");
  returnRam << Ram/1000;
  return returnRam.str(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string userID;
  std::stringstream path; 
  path << kProcDirectory << pid << kStatusFilename; 
  userID = getValues<string> (path.str(), "Uid:");
  return userID; 
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string userID = Uid(pid);
  string username = "user";
  std::ifstream filestream(kPasswordPath);
  
  if (filestream.is_open()) {
    std::string line;
    
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string currentID, xVal;
      
      if (linestream >> username >> xVal >> currentID) {
        if (currentID == userID) {
          return username;
        }
      }
    }
  }

  return username;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long uptimeClocks; 
  std::stringstream path; 
  path << kProcDirectory << pid << kStatFilename; 
  std::ifstream filestream(path.str());
  if(filestream.is_open()){
    const int position = 22;
    std::string value;
    for (int i = 0; i < position; i++) {
      if (!(filestream >> value)) {
        return 1000;
      }
    }
    uptimeClocks = std::stol(value);
  } // if filestream.is_open
  return uptimeClocks/sysconf(_SC_CLK_TCK);
}
