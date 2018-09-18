
#include "JobFinishedMessage.hpp"


Synergy::JobFinishedMessage::JobFinishedMessage(char *buffer, size_t length)
    : JobMessage(buffer, length, Message::Type::JobFinished)
{

}
