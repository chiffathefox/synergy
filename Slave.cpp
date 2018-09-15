
#include "Slave.hpp"


Synergy::Slave::Slave(const IPAddress &addr)
    : mHeartbeat(0)
{
    setAddr(addr);
}


void Synergy::Slave::setAddr(const IPAddress &addr)
{
    mAddr = addr;
}


IPAddress Synergy::Slave::addr() const
{
    return mAddr;
}


void Synergy::Slave::updateHeartbeat()
{
    mHeartbeat = millis();
}


unsigned long Synergy::Slave::heartbeat() const
{
    return mHeartbeat;
}
