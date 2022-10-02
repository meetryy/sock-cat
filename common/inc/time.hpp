#pragma once

#include <chrono>

using namespace std::chrono;
namespace epochTime{
    long int ms(void){
        milliseconds ms = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        );
        return (ms.count());
    }
}