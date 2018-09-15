
#pragma once


#include "Message.hpp"


namespace Synergy
{
    class JobMessage;
};


class Synergy::JobMessage : public Message
{

    struct JobMessageStruct
    {
        uint64_t jobId;
    };


    JobMessageStruct *mMessage;


public:

    explicit JobMessage(char *buffer, int length, Type type);

    uint64_t jobId() const;
    void setJobId(uint64_t jobId);

};
