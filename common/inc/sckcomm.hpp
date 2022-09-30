#pragma once
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <memory>
#include <queue>

#include "socket.hpp"
#include "../../common/inc/packet.hpp"
#include "../../common/inc/threadppol.hpp"
#include "../../common/inc/safequeue.hpp"

enum class Role {
    None,
    Server,
    Client,
    Generic
};

class Transceiver {
    enum class trState {
        NotInitialized,
        Initialized,
        Ready,
        PassingRead,
        Listening
    };

    SocketSystem sckSystem;
    Role role = Role::None;

    //std::thread listenerThread;
    
    std::string thisAddress;
    std::string thisPort;

    //int clientThread(void);
    //int packetReceiver(Socket &listenerSocket);
    
    class pktProcessor {
        SafeQueue<Packet> packetQ; 
        //std::queue<Packet> packetQ;
        std::thread workThread;

        public:
        int AddCandidate(std::string str);
        int AddDirectly(Packet p);
        int Process(Packet p);
        void Worker(void);
        void Stop(void);
        void Start(void);
    };

    //void (*genericTask);
    void (*genericTask)(void) = nullptr;
    //void* genericData;

    public:
    const unsigned int maxThreads = 16;
    pktProcessor pkt;
    ThreadPool tPool{maxThreads};
    trState state = trState::NotInitialized;

    //Transceiver(std::string addr, std::string port);
    Transceiver(const std::string& addr, const std::string& port);

    int Init(void);
    
    int joinThreads(void);

    //int StartListening(void);

    int setGenericTask(void (*f)());
    int serverTask(void); 
    int clientTask(void);

    int separateClientWorker(Socket &listenerSocket);

    

    int setRole(Role newRole);
    int playRole(void);

};


