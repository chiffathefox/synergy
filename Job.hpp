
#pragma once


#include <cstdint>

#include "Slave.hpp"
#include "JobSet.hpp"


namespace Synergy
{
    class Job;
};


class Synergy::Job
{

    static uint64_t sLastId;


    Slave *mSlave;
    JobSet *mJobSet;
    uint64_t mId;


public:

    Job();
    explicit Job(Slave *slave, JobSet *jobSet = nullptr);

    Slave *slave() const;
    JobSet *jobSet() const;
    uint64_t id() const;

    void finished();

};
