
#pragma once


#include <map>

#include "Job.hpp"
#include "NewJobMessage.hpp"


namespace Synergy
{
    class BroadcastJob;
};


class Synergy::BroadcastJob : public Job
{

    JobMessage *mMessage;
    std::map<Slave::id_t, Slave *> mSlaves;


    void propagate();


public:

    explicit BroadcastJob(MasterMode *master, const char *task,
            uint8_t taskLength);

    virtual void updateHeartbeat() override;
    virtual void emit() override;
    virtual JobMessage message() const override;
    virtual void finished(Slave *slave) override;

};
