
#pragma once


#include "Message.hpp"


namespace Synergy
{
    class JobMessage;
};


class Synergy::JobMessage : public Message
{

    struct MessageStruct
    {
        uint64_t jobId;
    };


    MessageStruct *mMessage;


public:

    explicit JobMessage(char *buffer, int length, Type type);


    inline uint64_t jobId() const
    {
        return mMessage->jobId;
    }


    inline void setJobId(uint64_t jobId)
    {
        mMessage->jobId = jobId;
    }


protected:

    virtual uint8_t offset() const override;

};
