
#pragma once


#include <cstddef>
#include <cstdint>


namespace Synergy
{
    class Message;
};


class Synergy::Message
{

    bool mOk;


public:


    enum Type : uint8_t
    {
        None = 0,
        SlaveBeacon,
        MasterBeacon,
        NewJob,
        JobFinished
    };


    Message(char *buffer = nullptr, int length = -1,
            Type type = Type::None);


    inline bool ok() const
    {
        return mOk;
    }


    inline Type type() const
    {
        return mMessage->type;
    }


protected:

    virtual uint8_t offset() const;


    inline char *buffer() const
    {
        return mMessage->buffer;
    }


    inline uint8_t bufferTotalSize() const
    {
        return sizeof (MessageStruct) - offsetof(MessageStruct, buffer);
    }


    inline uint8_t bufferAvailable() const
    {
        return bufferTotalSize() - offset();
    }


    inline void setOk(bool ok)
    {
        mOk = ok;
    }


    inline void setType(Type type)
    {
        mMessage->type = type;
    }


    inline uint8_t bufferLength() const
    {
        return mMessage->bufferLength;
    }


    inline void setBufferLength(uint8_t bufferLength)
    {
        mMessage->bufferLength = bufferLength;
    }


private:

    struct MessageStruct
    {
        Type type;
        uint8_t bufferLength;
        char buffer[64];
    };


    MessageStruct *mMessage;
    char mBuffer[sizeof (MessageStruct)];


public:


    static inline int maxRawLength()
    {
        return sizeof (MessageStruct);
    }


    inline char *raw() const
    {
        return reinterpret_cast<char *>(mMessage);
    }

    
    inline int rawLength() const
    {
        return offsetof(MessageStruct, buffer) + bufferLength();
    }


};
