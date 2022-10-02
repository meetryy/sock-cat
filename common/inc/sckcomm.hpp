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

// https://github.com/Pindrought/PNet/
// now only IPv4

class Endpoint {
    int port;
    int ipv = 0;
    int ipBytes[4];
    in_addr address;
    std::string ip;
    std::string hostname;
    bool resolved = 0;

    public:
    Endpoint(std::string ip, unsigned short port){
        this->port = port;
        int result = inet_pton(AF_INET, ip.c_str(), &address);
        if (result == 1) {
            if (address.S_un.S_addr != INADDR_NONE){
                this->ip = ip;
				hostname = ip;
                ipv = 4;
            }

            addrinfo hints = {};
            hints.ai_family = AF_INET;

            addrinfo* hostinfo = nullptr;

            result = getaddrinfo(ip.c_str(), NULL, NULL, &hostinfo);
            if (result == 0) {
                sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

                std::string ip_string(ip);
                ip_string.resize(16);
                inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);
    
                ULONG ip_long = host_addr->sin_addr.S_un.S_addr; //get ip address as unsigned long
                memcpy(&ipBytes[0], &ip_long, sizeof(ULONG)); //copy bytes into our array of bytes representing ip address
                ipv = 4;
                resolved = 1;
                freeaddrinfo(hostinfo); //memory cleanup from getaddrinfo call
            }
        }
    };
};

enum class Role {
    None,
    Server,
    Client,
    Generic
};

enum class pktDirection {Rx, Tx};
enum class pktType {ServerStatus, ClientStatus, MessageOut, MessageInResponse, trash};

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
        SafeQueue<Packet> rxPacketQ; 
        SafeQueue<Packet> txPacketQ;
        //std::queue<Packet> packetQ;
        std::thread workThread;
        std::string sessionId;

        public:
        pktProcessor();
        template<typename T> 
        std::string ConstructPacketString(pktType type, T payload, Packet& p);

        template<typename T> 
        std::string ConstructPacketString(pktType type, T payload);
        std::vector<Packet> parseStringToPackets(std::string& inputStr);
        int AddCandidateFromString(pktDirection type, std::string str);
        int AddCandidatePacket(pktDirection type, Packet p);
        int Process(Packet p);
        void Worker(void);
        void Stop(void);
        void Start(void);
        std::string getSessionId(void);
    };

    //void (*genericTask);
    void (*genericTask)(void) = nullptr;
    //void* genericData;

    //std::string sessionId;

    public:
    const unsigned int maxThreads = 16;
    pktProcessor pkt;
    ThreadPool tPool{maxThreads};
    trState state = trState::NotInitialized;
    //Endpoint e{"127.0.0.1", 25000};

    //Transceiver(std::string addr, std::string port);
    Transceiver(const std::string& addr, const std::string& port);

    int Init(void);
    
    int joinThreads(void);

    //int StartListening(void);

    int setGenericTask(void (*f)());
    int serverTask(void); 
    int clientTask(void);

    int separateClientWorker(Socket &listenerSocket);
    std::string getSessionId(void);


    int setRole(Role newRole);
    int playRole(void);

};


