#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iostream>

#include "../inc/json.hpp"
#include "../inc/packet.hpp"

Packet::Packet(){
    ok = false;
};

Packet::Packet(std::string str){
    if (json::accept(str)){
        j = json::parse(str);
        ok = true;
        s = str;
    };
};

Packet::~Packet(){
}

std::string Packet::getString(void){
    s = j.dump();
    return s;
}

void Packet::updString(void){
    s = j.dump();
}

bool Packet::isOk(void) {return ok;}


int Packet::receiveJson(json Jin){
    j = Jin;
}

