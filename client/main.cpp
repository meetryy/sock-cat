#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>

#include "../common/inc/packet.hpp"
#include "../common/inc/json.hpp"
#include "../common/inc/uuid.hpp"
#include "../common/inc/socket.hpp"
#include "../common/inc/sckcomm.hpp"
#include "../common/inc/log.hpp"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using json = nlohmann::json;

int __cdecl main(int argc, char **argv) { 
    
    Log textLog;
    //textLog.GetInstance().Add(0, "%s %s: start!", __FUNCTION__);
    //textLog.GetInstance().setOut(0, 1);

    Transceiver tr("127.0.0.1", "27015");
    

    tr.setRole(Role::Client);
    tr.playRole();
    


    /*
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char sendbuf[DEFAULT_BUFLEN];
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    PCSTR serverName = "127.0.0.1";
    // Resolve the server address and port
    iResult = getaddrinfo(serverName, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    srand(time(NULL));
    unsigned int clientID = rand()%100;

    int i = 1000;
    while (1) {
        i--;
        // Attempt to connect to an address until one succeeds
        ptr=result;
        //for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                printf("socket failed with error: %ld\n", WSAGetLastError());
                WSACleanup();
                return 1;
            }

            
            // Connect to server.
            iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                //continue;
            }
            //break;
        //}

        
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            return 1;
        }

        FD_SET WriteSet;
        FD_ZERO(&WriteSet);
        FD_SET(ConnectSocket, &WriteSet);

        struct timeval tv;
        tv.tv_usec = 1;

        printf("Waiting for be able to write...\n");
        select(0, NULL, &WriteSet, NULL, NULL);
        printf("Ready to write\n");

        unsigned int randomNum = rand()%999999;

        Packet a;

        a.addProperty("type", pktType::status);
        a.addProperty("content", "msgID", uuid::generate());
        a.addProperty("content", "clientInfo", "clientID", clientID);
        a.addProperty("content", "sessionInfo", "sessionID", randomNum);

        a.updString();
        std::cout << a.getString() << std::endl;
        // Send an initial buffer
        iResult = send(ConnectSocket, a.s.c_str(), (int)strlen(a.s.c_str()), 0 );
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        printf("Bytes Sent: %ld\n", iResult);


        // shutdown the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        do {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if ( iResult > 0 )
                printf("Bytes received: %d\n", iResult);
            else if ( iResult == 0 )
                printf("Connection closed\n");
            else
                printf("recv failed with error: %d\n", WSAGetLastError());

        } while( iResult > 0 );

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

    freeaddrinfo(result);
    WSACleanup();

    */

    return 0;
}