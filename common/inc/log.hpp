#pragma once
#include <ostream>
#include <thread>
#include <queue>
#include <sstream>
#include <fstream>
#include <mutex>

class Log{
    std::ofstream fileHandle;
    std::thread workThread;
    std::queue<std::string> writeQ;

   
    Log(const Log&) = delete; 
    Log& operator=(const Log&) = delete; 
    Log(Log&&) = delete; 
    Log& operator=(Log&&) = delete; 

    std::string currTimeDateStr(void);
    void checkAndUpdFileHandle(void);
    std::mutex Qmutex;

    int debugLevel = 0;
    bool logToConsole = 0;
    bool logToFile = 1;

    public: 
        Log();
        ~Log();
        static Log& GetInstance();
    int AddString(std::string in);
    int Open(std::string filename);
    void Worker(void);
    void Add(int level, const char* fmt, ...);
    void setDebugLevel(int level);
    void setOut(bool file, bool console);

/*
    template <typename... Ts>
    void Add( Ts&&... vars ){
        std::ostringstream str;
        if (!fill_stream(str, std::forward<Ts>(vars)...)){
            //Error!
        }
        std::string a = str.str();
        writeQ.push(a);
    }
*/

};

extern Log textLog;
    

