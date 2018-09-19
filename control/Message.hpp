
#pragma once


#include <cstddef>
#include <cstdint>

#include "DataProc.hpp"


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
        JobFinished,
        JobFinishedAccepted
    };


    Message(char *buffer = nullptr, size_t length = -1,
            Type type = Type::None);


    inline bool ok() const
    {
        return mOk;
    }


    inline Type type() const
    {
        return mMessage.type;
    }


protected:

    virtual uint8_t offset() const;


    inline char *buffer()
    {
        return mMessage.buffer;
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
        mMessage.type = type;
    }


    inline uint8_t bufferLength() const
    {
        return mMessage.bufferLength;
    }


    inline void setBufferLength(uint8_t bufferLength)
    {
        mMessage.bufferLength = bufferLength;
    }


private:

    struct synergy_align MessageStruct
    {
        Type type;
        uint8_t bufferLength;
        synergy_align char buffer[64];
    };


    MessageStruct mMessage;


public:


    static inline size_t maxRawLength()
    {
        return sizeof (MessageStruct);
    }


    inline const char *raw() const
    {
        return reinterpret_cast<const char *>(&mMessage);
    }

    
    inline size_t rawLength() const
    {
        return offsetof(MessageStruct, buffer) + bufferLength();
    }


};
