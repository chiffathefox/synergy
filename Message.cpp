
#include "Message.hpp"


Synergy::Message::Message(char *buffer, int length, Type type)
    : mOk(false),
    mBuffer(nullptr)
{
    if (length == -1) {
        mBuffer = new char[sizeof (MessageStruct)];
        mMessage = reinterpret_cast<MessageStruct *>(mBuffer);

        setType(type);
    }

    if (length < 2) {
        return;
    }

    mMessage = reinterpret_cast<MessageStruct *>(buffer);
    setOk(this->type() == type);
}


Synergy::Message::~Message()
{
    if (mBuffer != nullptr) {
        delete mBuffer;
    }
}


char *Synergy::Message::raw() const
{
    return reinterpret_cast<char *>(mMessage);
}


bool Synergy::Message::ok() const
{
    return mOk;
}


void Synergy::Message::setOk(bool ok)
{
    mOk = ok;
}


Synergy::Message::Type Synergy::Message::type() const
{
    return mMessage->type;
}


void Synergy::Message::setType(Type type)
{
    mMessage->type = type;
}


uint8_t Synergy::Message::bufferLength() const
{
    return mMessage->bufferLength;
}


void Synergy::Message::setBufferLength(uint8_t bufferLength)
{
    mMessage->bufferLength = bufferLength;
}


char *Synergy::Message::buffer() const
{
    return mMessage->buffer;
}
