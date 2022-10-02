#pragma once

#include <type_traits>
#include "json.hpp"

using json = nlohmann::json;

class Packet {
        bool ok;

    public:
        Packet();
        Packet(std::string str);

        ~Packet();

        json j;
        std::string s;
        int A;
        
        template<typename T> 
        void addProperty(std::string name, T t){
                j[name] = t;
        }

        template<typename T> 
        void addProperty(std::string name, std::string sub, T t){
                j[name][sub] = t;
        }

        template<typename T> 
        void addProperty(std::string name, std::string sub, std::string sub2, T t){
                j[name][sub][sub2] = t;
        }

        std::string getString(void);
        void updString(void);
        bool isOk(void);
        int receiveJson(json Jin);
};