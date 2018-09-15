
#pragma once


#include <ESP8266WiFi.h>


namespace Synergy
{
    class Slave;
};


class Synergy::Slave
{

    IPAddress mAddr;
    unsigned long mHeartbeat;


public:

    explicit Slave(const IPAddress &addr);

    void setAddr(const IPAddress &addr);
    IPAddress addr() const;

    void updateHeartbeat();
    unsigned long heartbeat() const;

};
