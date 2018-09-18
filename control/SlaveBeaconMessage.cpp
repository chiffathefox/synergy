
#include "SlaveBeaconMessage.hpp"


Synergy::SlaveBeaconMessage::SlaveBeaconMessage(char *message, size_t length)
    : Message(message, length, Message::Type::SlaveBeacon)
{
    if (message == nullptr) {
        setBufferLength(0);
    }
}
