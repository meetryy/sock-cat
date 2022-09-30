#pragma once
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

class SocketSystem {
    WSADATA wsaData;
    bool startupOk = false;

    public:
    int Startup(void);
    bool isOk(void);

};

#define DEFAULT_PORT "27015"
#define RX_BUF_LEN  (4096)

enum class SocketRole {
    None,
    Generic,
    Server,
    Client
};

class Socket {
    enum class sckState {
        NotInitialized,
        Resolved,
        Initialized,
        Bound,
        Connected,
        Ready,
        Listening,
        Accepted,
        Shutdown,
        Close,
        ResolveError,
        Error = -1,
    };


    SocketRole role = SocketRole::None;
    
    struct {
        int address[4];
        std::string addressStr;
        int port;
    } peerInfo;

    struct addrinfo *result = NULL;//, *ptr = NULL;
    struct addrinfo hints;
    int iResult;
    SOCKET socketObj = INVALID_SOCKET;

    std::string sckAddress;
    std::string sckPort;

    int ResolveAddress(void);
    int Init(void);
    int Bind(void);
    
    int CheckErr(int input, const std::string& text);

    const int recvbuflen = RX_BUF_LEN;
    char recvbuf[RX_BUF_LEN];
    bool blockingMode;

    public:
    
    Socket(std::string addr, std::string port);
    Socket(bool blocking);
    Socket();

    bool isReady(void);
    int prepare(SocketRole newRole);
    int PrepareServer(void);
    int PrepareClient(void);
    int Listen(void);
    int AcceptFromSock(SOCKET &s);
    int AcceptSelf(void);
    int Close(void);
    
    int isReadable(bool block);
    int isWriteable(bool block);
    bool isConnected(void);

    int setBlocking(bool block);

    int UpdPeerInfo(void);

    int ReceiveUntilClosed(void);
    std::string ReceiveString(void);
    
    int ConnectToAddr(std::string addr);
    int ConnectToLastResolved(void);
    int Shutdown(void);
    int Write(std::string& data);

    sckState state = sckState::NotInitialized;

    //void start(void);
    //SOCKET Socket::getObject(void);
    
    const   SOCKET& getObjectConstRef() const;
            SOCKET& getObjectRef();         
};

class SocketBuffer {


};