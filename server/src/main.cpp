#undef UNICODE
#include <winsock2.h>
#include <windows.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "../../common/inc/socket.hpp"
#include "../../common/inc/sckcomm.hpp"
#include "../inc/db.hpp"
#include "../inc/log.hpp"

void test(void){
    std::cout << "SSSSSSSSSSSS" <<std::endl;
};

int __cdecl main(void) 
{           

    Log textLog;
    textLog.GetInstance().Add(0, "MAIN: hello! this is a test!");
    textLog.GetInstance().setOut(0, 1);
    
    DB db;
    db.test();


    Transceiver tr("127.0.0.1", "27015");
    tr.Init();

    tr.setRole(Role::Server);
    //tr.setGenericTask(test);
    
    tr.playRole();
    
    //tr.joinThreads();

    return 0;
}