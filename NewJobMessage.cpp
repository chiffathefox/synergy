
#include "NewJobMessage.hpp"


Synergy::NewJobMessage::NewJobMessage(char *buffer, int length)
    : JobMessage(buffer, length, Message::Type::NewJob)
{

}
