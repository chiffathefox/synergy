
#include "Slave.hpp"


unsigned long Synergy::Slave::HeartbeatTimeout = 5000;
unsigned long Synergy::Slave::HeartbeatPeriod = 1000;


Synergy::Slave::Slave(const IPAddress &addr)
    : mAddr(addr),
    mHeartbeat(0)
{

}
