
#pragma once


#include "Message.hpp"
#include "Job.hpp"


namespace Synergy
{
    class JobMessage;
};


class Synergy::JobMessage : public Message
{

    struct MessageStruct
    {
        Job::id_t jobId;
    };


    MessageStruct *mMessage;


public:

    explicit JobMessage(char *buffer, int length, Type type);


    inline Job::id_t jobId() const
    {
        return mMessage->jobId;
    }


    inline void setJobId(Job::id_t jobId)
    {
        mMessage->jobId = jobId;
    }


protected:

    virtual uint8_t offset() const override;

};
