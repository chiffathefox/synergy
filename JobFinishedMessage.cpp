
#include "JobFinishedMessage.hpp"


Synergy::JobFinishedMessage::JobFinishedMessage(char *buffer, int length)
    : JobMessage(buffer, length, Message::Type::JobFinished)
{

}
