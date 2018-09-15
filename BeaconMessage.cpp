
#include "BeaconMessage.hpp"


Synergy::BeaconMessage::BeaconMessage(char *message, int length)
    : Message(message, length, Message::Type::Beacon)
{
    if (length == -1) {
        setBufferLength(0);
    }
}
