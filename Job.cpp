
#include "Job.hpp"


uint64_t Synergy::Job::sLastId = 0;


Synergy::Job::Job()
    : mSlave(nullptr),
    mJobSet(nullptr),
    mId(sLastId++)
{
}


Synergy::Job::Job(Slave *slave, JobSet *jobSet)
    : mSlave(slave),
    mJobSet(jobSet),
    mId(sLastId++)
{

}


Synergy::Slave *Synergy::Job::slave() const
{
    return mSlave;
}


Synergy::JobSet *Synergy::Job::jobSet() const
{
    return mJobSet;
}


uint64_t Synergy::Job::id() const
{
    return mId;
}


void Synergy::Job::finished()
{
    if (jobSet() != nullptr) {
        jobSet()->finished(*this);
    }
}
