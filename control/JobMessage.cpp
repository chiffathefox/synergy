
#include <Arduino.h>
#include "JobMessage.hpp"


Synergy::JobMessage::JobMessage(char *buffer, int length, Type type)
    : Message(buffer, length, type)
{
    mMessage = reinterpret_cast<MessageStruct *>(
            this->buffer() + Message::offset());


    if (length == -1) {
        setBufferLength(sizeof (MessageStruct));
        setJobId(0);

        return;
    }

    setOk(bufferLength() >= offset());
}


uint8_t Synergy::JobMessage::offset() const
{
    return Message::offset() + sizeof (MessageStruct);
}
