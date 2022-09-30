#include <chrono>
#include <thread>
#include <cstdlib>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime> 
#include <cstdarg>

#include "../inc/log.hpp"

std::string Log::currTimeDateStr(void){
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H-%M-%S");
    return ss.str();
};

void Log::setOut(bool file, bool console) {logToConsole = console; logToFile = file;}

void Log::Add(int level, const char* fmt, ...){
    /* Check if the message should be logged */
    //if (level > log_level)
    //    return;
    if (level <= debugLevel){
        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer,1024, fmt, args);
        perror(buffer);
        va_end(args);

        std::string a(buffer);

        if (logToFile) {
            Qmutex.lock();
            writeQ.push(a);
            Qmutex.unlock();
        }
        if (logToConsole) 
            std::cout << a << std::endl;
    }
};

Log::Log(){
    std::string filename = "log/" + currTimeDateStr() + ".log";

    fileHandle.open (filename, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!fileHandle ) 
        fileHandle.open(filename,  std::fstream::in | std::fstream::out | std::fstream::trunc);
    workThread = std::thread(Log::Worker, this);
    while (!fileHandle){
        
    }
};

Log& Log::GetInstance() {
    static Log* log = new Log();
    return *log;
}

void Log::checkAndUpdFileHandle(void){

}

void Log::setDebugLevel(int level){
    debugLevel = level; 
}

Log::~Log(){
    fileHandle.close();
}

int Log::AddString(std::string in){
    writeQ.push(in);
}

void Log::Worker(void){
    while(1){
        while (writeQ.empty() == false){
            fileHandle << currTimeDateStr() << " : " << writeQ.front() << "\n";
            fileHandle.flush();
            Qmutex.lock();
            writeQ.pop();
            Qmutex.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};
