#include <iostream>
#include <algorithm>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"
using namespace std;

class ProcessParser{
private:
    static std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
    static int getNumberOfCpuCores();
};

// TODO: Define all of the above functions below:
string ProcessParser::getCmd(string pid){
    string cmd = "";
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    getline(stream, cmd);
    return cmd;
}
/**
 * returns a string holding the value of the size of the vm
 */
string ProcessParser::getVmSize(string pid){
    string line =  "";
    string name = "VmData";
    float result = 0.0f;
    //We get the status path from the pid path
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            //We need a input stream iterator to acess the values in vm
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result = (stof(values[1])/float(1024));
            break;
        }
    }
    return to_string(result);
}
string ProcessParser::getCpuPercent(string pid){
    string line;
    float result;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statPath());
    getline(stream, line);
    istringstream buf (line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float start_time = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (start_time/freq);
    result = 100*((total_time/freq)/seconds);
    return to_string(result);
}

std::string ProcessParser::getProcUpTime(string pid){
    string value;
    string line;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statPath());
    getline(stream, line);
    istringstream buf (line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    return to_string(float(stof(values[13]))/sysconf(_SC_CLK_TCK));
}

long int ProcessParser::getSysUpTime(){
    string line;
    ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream, line);

    istringstream buf (line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

    return stoi(values[0]);
}

string ProcessParser::getProcUser(string pid){
    string line =  "";
    string name = "Uid";
    string result = "";
    //We get the status path from the pid path
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    
    while(getline(stream, line)){
        if(line.find(name) != std::string::npos){
            istringstream buf(line);
            //We need a input stream iterator to acess the values in vm
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result = values[1];
            break;
        }
    }

    name = "x:" + result;
    ifstream tream = Util::getStream("/etc/passwd-");
    
    while(getline(tream, line)){
        if(line.find(name) != std::string::npos){
            return result;
            break;
        }
    }
}
bool allDigit(string str){
    for(int i = 0; i < str.size(); i++){
        if(!isdigit(str[i]))return false;
    }
    return true;
}

vector<string> ProcessParser::getPidList(){
    DIR* dir;
    vector<string>container;
    if(!(dir = opendir("/proc"))){
        throw runtime_error(strerror(errno));
    }
    while(dirent* dirp = readdir(dir)){
        if(dirp->d_type != DT_DIR){
            continue;
        }
        if(allDigit(dirp->d_name)){
            container.push_back(dirp->d_name);
        }
    }

    if(closedir(dir)){
        throw runtime_error(strerror(errno));
    }
    return container;
}

int ProcessParser::getNumberOfCpuCores(){
    int result = 0;
    string line = "";
    string name = "cpu cores";
    ifstream stream = Util::getStream(Path::basePath() + "cpuinfo");
    while(getline(stream, line)){
        if(line.find(name) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            result = stoi(values[3]);
        }
    }
    return result;
}
vector<string> ProcessParser::getSysCpuPercent(string coreNumber){
    string line = "";
    string name = "cpu" + coreNumber;
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line)){
        if(line.find(name) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            return values;
        }
    }
    return (vector<string>());
}

float getSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string> values2){
    /**
     * CPU stats are time-dependent
     * So the only way to get valid CPU statistics are by specifying a time interval
     */
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

float ProcessParser::getSysRamPercent(){
    float sysRamPercent = 0.0f;
    string line = "";
    ifstream stream = Util::getStream(Path::basePath() + Path::memInfoPath());
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";

    float totalMem = 0;
    float freeMem = 0;
    float buffers = 0;

    while(getline(stream, line)){
        if(totalMem != 0 && freeMem != 0)break;
        if(line.find(name1) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            totalMem = stof(values[1]);
        }
        if(line.find(name2) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            freeMem = stof(values[1]);
        }
        if(line.find(name3) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            buffers = stof(values[1]);
        }
    }
    return (100*(1-(freeMem/(totalMem-buffers))));
}

string ProcessParser::getSysKernelVersion(){
    string line = "";
    string name = "Linux";
    ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            return values[2];
        }
    }
    return name;
}

string ProcessParser::getOSName(){
    ifstream stream = Util::getStream("/etc/os-release");
    string OSName = "";
    string name = "PRETTY_NAME=";
    string line = "";
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            OSName = line.substr(name.size());
            return OSName;
        }
    }
    return OSName;
}
int ProcessParser::getTotalThreads(){
    string name = "Threads";
    string line= "";
    int totalThreads = 0;
    for(auto pid : ProcessParser::getPidList()){
        ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
        while(getline(stream, line)){
            if(line.find(name) != string::npos){
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                totalThreads += stoi(values[1]);
            }
        }
    }
    return totalThreads;
}

int ProcessParser::getTotalNumberOfProcesses(){
    string name = "processes";
    string line= "";
    int totalProcesses = 0;
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalProcesses = stoi(values[1]);
        }
    }
    return totalProcesses;
}

int ProcessParser::getNumberOfRunningProcesses(){
    string name = "procs_running";
    string line= "";
    int totalProcesses = 0;
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalProcesses = stoi(values[1]);
        }
    }
    return totalProcesses;
}

bool ProcessParser::isPidExisting(string pid){
    for(auto i : getPidList()){
        if(i == pid)return true;
    }
    return false;
}