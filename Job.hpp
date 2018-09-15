
#pragma once


#include <cstdint>

#include "Slave.hpp"
#include "JobMessage.hpp"


namespace Synergy
{
    class MasterMode;
    class Job;
};


#include "MasterMode.hpp"


class Synergy::Job
{

    static uint64_t sLastId;


    MasterMode *mMaster;
    const char *mTask;
    uint8_t mTaskLength;
    Slave *mSlave;
    uint64_t mId;
    unsigned long mHeartbeat;


public:


    inline static uint64_t getId()
    {
        return sLastId++;
    }


    explicit Job(MasterMode *master, const char *task, uint8_t taskLength);
    virtual ~Job();

    virtual void updateHeartbeat();

    virtual void emit() = 0;
    virtual JobMessage message() const = 0;
    virtual void finished(uint64_t id, Slave *slave) = 0;


    inline MasterMode *master() const
    {
        return mMaster;
    }


    inline const char *task() const
    {
        return mTask;
    }


    inline uint8_t taskLength() const
    {
        return mTaskLength;
    }


    inline Slave *slave() const
    {
        return mSlave;
    }


    inline void setSlave(Slave *slave)
    {
        mSlave = slave;
    }


    inline uint64_t id() const
    {
        return mId;
    }


    inline unsigned long heartbeat() const
    {
        return mHeartbeat;
    }


};
