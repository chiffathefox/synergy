
#include "JobFinishedAcceptedMessage.hpp"


Synergy::JobFinishedAcceptedMessage::JobFinishedAcceptedMessage(
        char *buffer, size_t length)
    : JobMessage(buffer, length, Message::Type::JobFinishedAccepted)
{

}
