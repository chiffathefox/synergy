
#include "SlaveBeaconMessage.hpp"


Synergy::SlaveBeaconMessage::SlaveBeaconMessage(char *message, int length)
    : Message(message, length, Message::Type::SlaveBeacon)
{
    if (length == -1) {
        setBufferLength(0);
    }
}
