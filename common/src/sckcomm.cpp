#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <type_traits>

#include "../../common/inc/packet.hpp"
#include "../../common/inc/uuid.hpp"
#include "../../common/inc/socket.hpp"
#include "../../common/inc/sckcomm.hpp"
#include "../../common/inc/debug.hpp"
#include "../../common/inc/time.hpp"

//std::vector<std::thread> myThreads;

Transceiver::Transceiver(const std::string& addr, const std::string& port){
    Init();
    thisAddress = addr;
    thisPort = port;
    
    
    ////textLog.GetInstance().Add(0, "%s: created transceiver %s %s", __FUNCTION__, thisAddress, thisPort);
    // TODO: add validation!
};

int Transceiver::Init(void){
    ////textLog.GetInstance().Add(0, "%s: initializing trx", __FUNCTION__);
    sckSystem.Startup();
    if (!sckSystem.isOk())
        return -1;
    //pkt.sessionId = uuid::generate();
    state = trState::Initialized;
    ////textLog.GetInstance().Add(0, "%s: trx init ok", __FUNCTION__);
    ////textLog.GetInstance().Add(0, "%s: starting tpool threads...", __FUNCTION__);
    tPool.Start();

}

int Transceiver::setGenericTask(void (*f)()){
    genericTask = f;
}

int Transceiver::setRole(Role newRole){
    ////textLog.GetInstance().Add(0, "%s: setting trx role. new role is %i, old is %i", __FUNCTION__, newRole, role);
    int result = 0;
    if (state == trState::Initialized){
        if (role == Role::None){
            if (newRole == Role::Client){
                role = newRole;
            }
            else if (newRole == Role::Server){
                role = newRole;
            }
            else {
                ////textLog.GetInstance().Add(0, "%s: error: requested role is unknown!", __FUNCTION__);
                result = -2;
            }
        }
        else {
            ////textLog.GetInstance().Add(0, "%s: error: role is already set!", __FUNCTION__);
            result = -1;
        }
    }
    else {
        ////textLog.GetInstance().Add(0, "%s: error: trx is not initialized!", __FUNCTION__);
        result = -4;
    }
    ////textLog.GetInstance().Add(0, "%s: role set result is %i", __FUNCTION__, result);
    return result;
}

int Transceiver::playRole(void){
    ////textLog.GetInstance().Add(0, "%s: playing trx role...", __FUNCTION__);
    int result = 0;

    if (role == Role::Client)
        clientTask();

    else if (role == Role::Server)
        serverTask();

    else if (role == Role::Generic){
        if (genericTask != nullptr)
            genericTask();
    }

    else result = -2;
    return result;
}

/*
int Transceiver::StartListening(void){
    // start listener
    ////textLog.GetInstance().Add(0, "%s: starting trx listener thread and packet worker thread...", __FUNCTION__);
    
    //listenerThread = std::thread(serverTask, this);
    packetWorkThread = std::thread(packetWorker, this);

    //listenerTask();
    // start worker threads
    
    
    //tPool.QueueJob([this] {this->packetWorker();});
}
*/

void Transceiver::pktProcessor::Stop(void){
    workThread.join();
}

void Transceiver::pktProcessor::Start(void){
    workThread = std::thread(Worker, this);
}

int Transceiver::joinThreads(void){
    ////textLog.GetInstance().Add(0, "%s: joining trx threads...", __FUNCTION__);
    //listenerThread.join();
    pkt.Stop();
}

int Transceiver::clientTask(void){
    ////textLog.GetInstance().Add(0, "%s: running client task of trx...", __FUNCTION__);
    Socket sender(thisAddress, thisPort);
    sender.prepare(SocketRole::Client);

    if (sender.isReady()){
        ////textLog.GetInstance().Add(0, "%s: sender is ready, trying to connect", __FUNCTION__);

        while (sender.ConnectToLastResolved() != 0){
            ////textLog.GetInstance().Add(0, "%s: attempting connect to %s:%s", __FUNCTION__, thisAddress.c_str(), thisPort.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        while(1) {
            if (sender.isWriteable(true)){
                unsigned int randomNum = rand()%999999;

                Packet a;
                std::string u = uuid::generate();
                ////textLog.GetInstance().Add(0, "%s: sending msg %s", __FUNCTION__, u.c_str());
                
                //a.addProperty("type", pktType::status);
                a.addProperty("msgID", u);
                //a.addProperty("content", "clientInfo", "clientID", 0);
               // a.addProperty("content", "sessionInfo", "sessionID", randomNum);
                //a.updString();
                //
                std::string A = a.getString();
                sender.Write(A);
                std::cout << A << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                //tPool.QueueJob([this, &listener] {this->separateClientWorker(listener);});
               // separateClientWorker(listener);


                /*
                newConn.AcceptFromSock(listener.getObjectRef());
                std::string a = newConn.ReceiveString();
                addPacketCandidate(a);
                */
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }   

} 

//int Transceiver::genericTask(void);

int Transceiver::serverTask(void){
    ////textLog.GetInstance().Add(0, "%s: running server task of trx...", __FUNCTION__);
    
    Socket listener(thisAddress, thisPort);
    listener.prepare(SocketRole::Server);

    if (listener.isReady()){
        ////textLog.GetInstance().Add(0, "%s: listener is ready. starting listening...", __FUNCTION__);
        listener.Listen();
        state = trState::Listening;
        pkt.Start();
        //Socket newConn;
        //newConn.prepare(SocketRole::Generic);
        while(1) {
            if (listener.isReadable(true)){
                //textLog.GetInstance().Add(0, "%s: listener is readable. reading...", __FUNCTION__);
                state = trState::PassingRead;
                
                std::cout << "creating separateClientWorker" << std::endl;

                tPool.QueueJob([this, &listener] {this->separateClientWorker(listener);});

                //separateClientWorker(listener);


                /*
                newConn.AcceptFromSock(listener.getObjectRef());
                std::string a = newConn.ReceiveString();
                addPacketCandidate(a);
                */
            }
        }
    }   

}

/*
int Transceiver::packetReceiver(Socket &listenerSocket){

}
*/

int Transceiver::separateClientWorker(Socket &listenerSocket){
    ////textLog.GetInstance().Add(0, "%s: starting separateClientWorker", __FUNCTION__);
    // will run in separate thread, accept socket and save string via addPacketCandidate 
    Socket thisClientConn;

    thisClientConn.AcceptFromSock(listenerSocket.getObjectRef());
    //if (thisClientConn.setBlocking(false) != 0)
    //     std::cout << "can't set blocking" <<std::endl;

    while (thisClientConn.isConnected()){
        //std::cout << "connected" <<std::endl;
        if (thisClientConn.isReadable(true)){
            std::string a = thisClientConn.ReceiveString();
            std::vector<Packet> packets = pkt.parseStringToPackets(a);
            for (auto newPacket : packets){           
                std::cout << newPacket.getString() <<std::endl;
            }
            //Packet newPacket(a);
            //
            //if (pkt.AddCandidatePacket(pktDirection::Rx, newPacket))
            //    pkt.ConstructPacketString(pktType::MessageInResponse, "", newPacket);
        }   
        
        /*
        std::string ok = "OK!";
        if (thisClientConn.isWriteable(false)){
            thisClientConn.Write(ok);
        }
        */
    }


    thisClientConn.Close();
}


std::vector<Packet> Transceiver::pktProcessor::parseStringToPackets(std::string& inputStr){
    std::stringstream ss;
    ss << inputStr; 
    std::vector<Packet> toReturn;
    while ((!ss.eof()) && (inputStr != "")) {
        json jmsg;
        ss >> jmsg;
        //std::cout << jmsg << std::endl;
        std::string jStr = jmsg.dump();
        Packet newPacket(jStr);
        toReturn.push_back(newPacket);
        size_t start_position_to_erase = inputStr.find(jStr);
        inputStr.erase(start_position_to_erase, jStr.length());
    }
    return toReturn;
}


void Transceiver::pktProcessor::Worker(void){
    ////textLog.GetInstance().Add(0, "%s: starting packetWorker", __FUNCTION__);
    while (1) {
        while (rxPacketQ.empty() == false){
            Packet a;
            rxPacketQ.dequeue(a);
            Process(a);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int Transceiver::pktProcessor::Process(Packet p){
    switch(static_cast<pktType>(p.j["type"])){
        /*
        case pktType::message: {
            std::cout << "msg in!" << p.j["someRandomShit"] << std::endl;
            break;
        }
        case pktType::status: {
            std::cout << "status in:" << std::endl;
            std::cout << std::setw(4) << p.j.dump() << std::endl;
         
            std::cout << "message ID: " << p.j["content"]["msgID"] << std::endl;
            std::cout << "client ID: " << p.j["content"]["clientInfo"]["clientID"] << std::endl;
            std::cout << "session ID: " << p.j["content"]["sessionInfo"]["sessionID"] << std::endl;
         
            break;
        }
        */
        case pktType::trash: {
            std::cout << "trash in!" << p.j["someRandomShit"] << std::endl;
            break;
        }
        default: break;
    }
    return 0;
}

int Transceiver::pktProcessor::AddCandidateFromString(pktDirection type, std::string str){
    Packet packCand(str);
    if (packCand.isOk() == false)
        return -1;

    if (type == pktDirection::Rx)
        rxPacketQ.enqueue(packCand);
    else if (type == pktDirection::Tx)
        txPacketQ.enqueue(packCand);
    return 0;
}

int Transceiver::pktProcessor::AddCandidatePacket(pktDirection type, Packet p){
    if (type == pktDirection::Rx)
        rxPacketQ.enqueue(p);
    else if (type == pktDirection::Tx)
        txPacketQ.enqueue(p);
}

Transceiver::pktProcessor::pktProcessor(){
    sessionId = uuid::generate();
}

std::string Transceiver::pktProcessor::getSessionId(void){
    return sessionId;
};

template<typename T>
std::string Transceiver::pktProcessor::ConstructPacketString(pktType type, T payload, Packet& p){
    Packet newPacket;
    newPacket.addProperty("sessionId", sessionId);
    newPacket.addProperty("msgId", uuid::generate());
    newPacket.addProperty("time", epochTime::ms());
    
    // ClientStatus, MessageOut, MessageInResponse, trash
    switch (type){
        case pktType::ServerStatus:{
            
            break;
        }
        case pktType::ClientStatus:{
            
            break;
        }
        case pktType::MessageOut:{
            newPacket.addProperty("msgText", payload);
            break;
        }
        case pktType::MessageInResponse:{
            newPacket.addProperty("responseToId", p.j["msgId"]);
            newPacket.addProperty("responseResult", payload);
            break;
        }
        case pktType::trash:{
            newPacket.addProperty("someTrash", static_cast<int>(rand() % 100));
            break;
        }
        default: break;
    }

    newPacket.updString();
    return (newPacket.getString());
}
