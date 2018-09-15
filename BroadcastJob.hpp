
#pragma once


#include <vector>

#include "Job.hpp"
#include "NewJobMessage.hpp"


namespace Synergy
{
    class BroadcastJob;
};


class Synergy::BroadcastJob : public Job
{

    NewJobMessage mMessage;
    std::vector<Slave *> mSlaves;


public:

    explicit BroadcastJob(MasterMode *master, const char *task,
            uint8_t taskLength);

    virtual void updateHeartbeat() override;
    virtual void emit() override;
    virtual JobMessage message() const override;
    virtual void finished(uint64_t id, Slave *slave) override;

};
