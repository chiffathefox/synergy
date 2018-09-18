
#pragma once


#include <cstdint>

#include "Slave.hpp"


namespace Synergy
{
    class MasterMode;
    class JobMessage;
    class Job;
};


class Synergy::Job
{

    MasterMode *mMaster;
    const char *mTask;
    uint8_t mTaskLength;
    Slave *mSlave;
    unsigned long mHeartbeat;


public:


    typedef short id_t;


    /*
     * Generic Arduino Serial class has a 64 byte long buffer,
     * which means that we can't spit out large chunks at once.
     * ... So we don't make any...
     *
     * 64 - (4 + 5 + 1 + 2) - (1 + 3) - 1 = 47
     */

    static const uint8_t MaxTaskLength = 47;
    static const unsigned long HeartbeatPeriod = 50;


    inline static id_t getId()
    {
        return sLastId++;
    }


    explicit Job(MasterMode *master, const char *task, uint8_t taskLength);
    virtual ~Job();

    virtual void updateHeartbeat();

    virtual void emit() = 0;
    virtual JobMessage message() const = 0;
    virtual void finished(Slave *slave) = 0;


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


    inline id_t id() const
    {
        return mId;
    }


    inline unsigned long heartbeat() const
    {
        return mHeartbeat;
    }


private:

    static id_t sLastId;


    id_t mId;

};


#include "MasterMode.hpp"
#include "JobMessage.hpp"
