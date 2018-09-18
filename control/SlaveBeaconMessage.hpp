
#pragma once


#include "Message.hpp"


namespace Synergy
{
    class SlaveBeaconMessage;
};


class Synergy::SlaveBeaconMessage : public Message
{


public:

    SlaveBeaconMessage(char *buffer = nullptr, size_t length = -1);

};
