
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


    typedef uint8_t id_t;


    static const unsigned long HeartbeatTimeout = 1000;
    static const unsigned long HeartbeatPeriod = 50;


    explicit Slave(const IPAddress &addr);


    inline void updateHeartbeat()
    {
        mHeartbeat = millis();
    }


    inline unsigned long heartbeat() const
    {
        return mHeartbeat;
    }


    inline IPAddress addr() const
    {
        return mAddr;
    }


    inline id_t id() const
    {
        return addr()[3];
    }


    inline bool alive() const
    {
        return millis() - heartbeat() < HeartbeatTimeout;
    }


};
