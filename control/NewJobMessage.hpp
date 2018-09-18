
#pragma once


#include "JobMessage.hpp"


namespace Synergy {
    class NewJobMessage;
};


class Synergy::NewJobMessage : public JobMessage
{


    struct synergy_align MessageStruct
    {
        uint8_t taskLength;
        char task[Synergy::Job::MaxTaskLength];
    };

    
    MessageStruct *mMessage;


public:

    NewJobMessage(char *buffer = nullptr, size_t length = -1);


    void setTask(const char *task, uint8_t taskLength);


    inline uint8_t taskLength() const
    {
        return mMessage->taskLength;
    }


    inline char *task() const
    {
        return mMessage->task;
    }


protected:

    virtual uint8_t offset() const override;


};
