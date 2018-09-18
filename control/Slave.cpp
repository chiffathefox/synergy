
#include "Slave.hpp"


Synergy::Slave::Slave(const IPAddress &addr)
    : mAddr(addr),
    mHeartbeat(0)
{

}
