
#include "Message.hpp"


Synergy::Message::Message(char *buffer, int length, Type type)
    : mOk(false),
    mBuffer(nullptr)
{
    if (length == -1) {
        mBuffer = new char[sizeof (MessageStruct)];
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


Synergy::Message::~Message()
{
    if (mBuffer != nullptr) {
        delete mBuffer;
    }
}


uint8_t Synergy::Message::offset() const
{
    return 0;
}
