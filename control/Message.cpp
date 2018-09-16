
#include "Message.hpp"


Synergy::Message::Message(char *buffer, int length, Type type)
    : mOk(false)
{
    if (length == -1) {
        mMessage = reinterpret_cast<MessageStruct *>(mBuffer);

        setType(type);
        setBufferLength(0);
    }

    if (length < 2) {
        return;
    }

    mMessage = reinterpret_cast<MessageStruct *>(buffer);
    setOk(this->type() == type || type == Type::None);
}


uint8_t Synergy::Message::offset() const
{
    return 0;
}
