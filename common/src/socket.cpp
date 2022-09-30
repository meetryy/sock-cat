#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "../inc/socket.hpp"
#include "../../common/inc/log.hpp"

#include <ostream>

int SocketSystem::Startup(void) {
    //textLog.GetInstance().Add(0, "%s: socket system startup...", __FUNCTION__);
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        //textLog.GetInstance().Add(0, "%s: WSA startup failed: %d", __FUNCTION__, iResult);
        return -1;
    }
    else startupOk = true;
    //textLog.GetInstance().Add(0, "%s: socket system startup OK", __FUNCTION__);
    return 0;
}

bool SocketSystem::isOk(void) {return startupOk;}

int Socket::setBlocking(bool block){
    unsigned long Blocking = 0;
    unsigned long NonBlocking = 0;
    
    u_long mode = static_cast<u_long>(block);  // 1 to enable non-blocking socket
    iResult = ioctlsocket(socketObj, FIONBIO, block ? &Blocking : &NonBlocking);
    if (CheckErr(iResult, __FUNCTION__)){
        return -1;
    }
    blockingMode = block;
    return 0;
}

int Socket::ResolveAddress(void){
    //textLog.GetInstance().Add(0, "%s: resolving socket address", __FUNCTION__);

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(sckAddress.c_str(), sckPort.c_str(), &hints, &result);
    if (iResult != 0) {
        //textLog.GetInstance().Add(0, "%s: getaddrinfo failed: %d", __FUNCTION__, iResult);
        WSACleanup();
        state = sckState::ResolveError;
        //textLog.GetInstance().Add(0, "%s: WSAcleanup done", __FUNCTION__);
        return -1;
    }
    state = sckState::Resolved;
    //textLog.GetInstance().Add(0, "%s: socket resolved OK", __FUNCTION__);
    return 0;
}

int Socket::Init(void){   
    //textLog.GetInstance().Add(0, "%s: socket init...", __FUNCTION__);
    socketObj = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (CheckErr(iResult, "initialization")) {
        freeaddrinfo(result);
        return -1;
    }

    state = sckState::Initialized;
    //textLog.GetInstance().Add(0, "%s: socket init OK", __FUNCTION__);
    return 0;
}

Socket::Socket(std::string addr, std::string port){
    //textLog.GetInstance().Add(0, "%s: constructing socket with %s:%s", __FUNCTION__, addr.c_str(), port.c_str());
    // TODO: add validation!
    sckAddress = std::string(addr);
    sckPort = std::string(port);
}

/*
Socket::Socket(const std::string& addr, const std::string& port){
    sckAddress = addr;
    sckPort = port;
}
*/

Socket::Socket(bool blocking){
    setBlocking(blocking);
}


Socket::Socket(){
    //textLog.GetInstance().Add(0, "%s: constructing generic socket", __FUNCTION__);
    //Prepare();
}


int Socket::Close(void){
    //textLog.GetInstance().Add(0, "%s: closing socket...", __FUNCTION__);
    closesocket(socketObj);
    //WSACleanup();

    state = sckState::Close;
}
 
int Socket::Bind(void){
    // Setup the TCP listening socket
    //textLog.GetInstance().Add(0, "%s: trying to bind", __FUNCTION__);
    iResult = bind(socketObj, result->ai_addr, (int)result->ai_addrlen);

    if (CheckErr(iResult, "bind")) {
        freeaddrinfo(result);
        return -1;
    }
    
    freeaddrinfo(result);
    state = sckState::Bound;
    //textLog.GetInstance().Add(0, "%s: bind OK", __FUNCTION__);
    return 0;
}

#include <iostream>
int Socket::CheckErr(int input, const std::string& text){
    if (input == SOCKET_ERROR) {
        //printf("%d failed with error: %d\n", text, );
        int i = WSAGetLastError();
        char msgbuf [256];   // for a message up to 255 bytes.
        msgbuf [0] = '\0';    // Microsoft doesn't guarantee this on man page.

        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
               NULL,                // lpsource
               i,                 // message id
               MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US),    // languageid
               msgbuf,              // output buffer
               sizeof (msgbuf),     // size of msgbuf, bytes
               NULL);               // va_list of arguments


        //textLog.GetInstance().Add(0, "%s: %s failed with error %i : %s", __FUNCTION__, text.c_str(), i, msgbuf);
        //closesocket(socketObj);
        //WSACleanup();
        state = sckState::Error;
        return -1;
    }

    return 0;
}

int Socket::Listen(void){
    iResult = listen(socketObj, SOMAXCONN);
    if (CheckErr(iResult, "listen")) 
        return -1;
        
    state = sckState::Listening;
    //textLog.GetInstance().Add(0, "%s: listening OK", __FUNCTION__);
    return 0;
}


bool Socket::isConnected(void){
    int r = recv(socketObj, NULL, 0, 0);
    return (r != SOCKET_ERROR && WSAGetLastError() != WSAECONNRESET);
}

int Socket::AcceptFromSock(SOCKET &s){
    ////textLog.GetInstance().Add(0, "%s: accepting connection from another socket", __FUNCTION__);

    struct sockaddr_in sa = {0};
    socklen_t socklen = sizeof sa;
    socketObj = accept(s, (struct sockaddr*) &sa, &socklen);
    if (socketObj == INVALID_SOCKET) {
        //textLog.GetInstance().Add(0, "%s: accept failed with error: %d", __FUNCTION__,  WSAGetLastError());
        closesocket(socketObj);
        WSACleanup();
        state = sckState::Error;
        return -1;
    }

    peerInfo.address[0] = sa.sin_addr.S_un.S_un_b.s_b1;
    peerInfo.address[1] = sa.sin_addr.S_un.S_un_b.s_b2;
    peerInfo.address[2] = sa.sin_addr.S_un.S_un_b.s_b3;
    peerInfo.address[3] = sa.sin_addr.S_un.S_un_b.s_b4;
    peerInfo.port       = sa.sin_port;

    //textLog.GetInstance().Add(0, "%s: accepting done. peer addr is %i.%i.%i.%i:%i", __FUNCTION__,  peerInfo.address[0], peerInfo.address[1], peerInfo.address[2], peerInfo.address[3], peerInfo.port);
    state = sckState::Accepted;
    return 0;
}

int Socket::AcceptSelf(void){
    //textLog.GetInstance().Add(0, "%s: accepting connection from self", __FUNCTION__);
    socketObj = accept(socketObj, NULL, NULL);
    if (socketObj == INVALID_SOCKET) {
        //textLog.GetInstance().Add(0, "%s: accept failed with error: %d", __FUNCTION__,  WSAGetLastError());
        closesocket(socketObj);
        WSACleanup();
        return 1;
    }
}

int Socket::prepare(SocketRole newRole){
    //textLog.GetInstance().Add(0, "%s: setting socket role. new role is %i, old is %i", __FUNCTION__, newRole, role);
    int result = 0;
    if (role == SocketRole::None){
        if (newRole == SocketRole::Client){
            if (PrepareClient() == 0)
                role = newRole;
            else {
                //textLog.GetInstance().Add(0, "%s: error: PrepareClient failed", __FUNCTION__);
                result = -3;
            }
        }

        else if (newRole == SocketRole::Server){
            if (PrepareServer() == 0)
                role = newRole;
            else {
                //textLog.GetInstance().Add(0, "%s: error: PrepareServer failed", __FUNCTION__);
                result = -4;
            }
        }
        else {
            //textLog.GetInstance().Add(0, "%s: error: new role is unknown!", __FUNCTION__);
            result = -2;
        }
    }
    else {
        //textLog.GetInstance().Add(0, "%s: error: role is already set!", __FUNCTION__);
        result = -1;
    }
    return result;
}

int Socket::PrepareServer(void){
    int res = -1;
    // preparing socket as a server: construct -> bind -> listen -> pass to new Socket on connection -> read/write -> close
    //textLog.GetInstance().Add(0, "%s: preparing server...", __FUNCTION__);
    res = ResolveAddress();
    if (res != 0) return -1;
    
    res = Init();
    if (res != 0) return -1;
    
    res = Bind();
    if (res != 0) return -1;

    if (state != sckState::Bound) return -1;    
    
    state = sckState::Ready;
    //textLog.GetInstance().Add(0, "%s: server ready", __FUNCTION__);
    return 0;
}


int Socket::PrepareClient(void){
    // preparing socket as a client: construct -> connect -> write/read -> close
    //textLog.GetInstance().Add(0, "%s: preparing client...", __FUNCTION__);
    if (ResolveAddress() == 0)
        this->Init();
    if (state != sckState::Initialized)
        return -1;
    state = sckState::Ready;
    //textLog.GetInstance().Add(0, "%s: client ready", __FUNCTION__);
    return 0;
}


int Socket::ConnectToAddr(std::string addr){
    //textLog.GetInstance().Add(0, "%s: trying to connect to %s", __FUNCTION__, addr.c_str());
    iResult = connect(socketObj, result->ai_addr, (int)result->ai_addrlen);
    if (CheckErr(iResult, "ConnectToAddr")) {
        //socketObj = INVALID_SOCKET;
        return -1;
    }
    state = sckState::Connected;
    //textLog.GetInstance().Add(0, "%s: connected OK", __FUNCTION__);
    return 0;
}

int Socket::ConnectToLastResolved(void){
    //textLog.GetInstance().Add(0, "%s: connecting to last resolved address...", __FUNCTION__);
    int result = ConnectToAddr(sckAddress);
    return result;
}

int Socket::Shutdown(void){
    //textLog.GetInstance().Add(0, "%s: shutting socket down...", __FUNCTION__);
    iResult = shutdown(socketObj, SD_SEND);
    if (CheckErr(iResult, "Shutdown")) {
        WSACleanup();
        return -1;
    }
    return 0;
}

int Socket::Write(std::string& data){
    std::string str(data);
    const char* c = str.c_str();   
    //textLog.GetInstance().Add(0, "%s: sending msg", __FUNCTION__);
    iResult = send(socketObj, c, (int)strlen(c), 0);

    if (CheckErr(iResult, __FUNCTION__)) {
        textLog.GetInstance().Add(0, "%s: send failed!", __FUNCTION__);
        //WSACleanup();
        return -1;
    }

    return 0;
}

bool Socket::isReady(void){
    return (state == sckState::Ready);
}

int Socket::UpdPeerInfo(void){
   if (state == sckState::Connected){

   }
}

int Socket::isReadable(bool block){
    DWORD result = 0;

    FD_SET ReadSet;
    FD_ZERO(&ReadSet);
    FD_SET(socketObj, &ReadSet);

    if (block)
        select(0, &ReadSet, NULL, NULL, NULL);
    else {
        struct timeval tv;
        tv.tv_usec = 1;
        select(0, &ReadSet, NULL, NULL, &tv);
    }

    if (result == SOCKET_ERROR)
        return -1;
    
    if (FD_ISSET(socketObj, &ReadSet)) 
        result = 1;

    return (int)result;
}

int Socket::isWriteable(bool block){
    DWORD result = 0;

    FD_SET WiteSet;
    FD_ZERO(&WiteSet);
    FD_SET(socketObj, &WiteSet);

    if (block)
        select(0, NULL, &WiteSet,  NULL, NULL);
    else {
        struct timeval tv;
        tv.tv_usec = 1;
        select(0, NULL, &WiteSet, NULL, &tv);
    }

    if (result == SOCKET_ERROR)
        return -1;
    
    if (FD_ISSET(socketObj, &WiteSet)) 
        result = 1;

    return (int)result;
}

int Socket::ReceiveUntilClosed(void){
    DWORD result;
    do {
        result = recv(socketObj, recvbuf, recvbuflen, 0);
        if ( result > 0 )
            printf("Bytes received: %d\n", result);
        else if ( result == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( result > 0 );
}

std::string Socket::ReceiveString(void){
    DWORD result;

    memset(recvbuf, 0, recvbuflen);
    do {
        result = recv(socketObj, recvbuf, recvbuflen, 0);
        if (result > 0)
            printf("Bytes received: %d\n", result);
        //else if ( result == 0 )
        //   printf("Connection closed\n");
        //if ( result >= 0 )
        //printf("recv failed with error: %d\n", WSAGetLastError());

    } while (result > 0);
    /*
    if ( result == 0 )
        printf("Connection closed\n");
    */

    std::string s = "";
    if (result >= 0)
        return std::string(recvbuf);
    else return 0;
}


const   SOCKET& Socket::getObjectConstRef() const   {return socketObj;}
        SOCKET& Socket::getObjectRef()              {return socketObj;}