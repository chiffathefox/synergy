
#pragma once


#include "Message.hpp"


namespace Synergy
{
    class BeaconMessage;
};


class Synergy::BeaconMessage : public Message
{


public:

    explicit BeaconMessage(char *buffer, int length);

};
