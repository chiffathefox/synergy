
#include <sstream>

#include "StringUtils.hpp"
#include "Job.hpp"


Synergy::Job::id_t Synergy::Job::sLastId = 1;


Synergy::Job::Job(MasterMode *master, const char *task, uint8_t taskLength)
    : mMaster(master),
    mTask(task),
    mTaskLength(taskLength),
    mSlave(nullptr),
    mHeartbeat(0),
    mFinishOnSlavesDeath(false),
    mId(getId())
{

}


Synergy::Job::~Job()
{

}


void Synergy::Job::updateHeartbeat()
{
    mHeartbeat = millis();
}


void Synergy::Job::emit()
{
    Job::updateHeartbeat();
}


std::string Synergy::Job::toString() const
{
    return std::string("Job #") + StringUtils::toString(id()) +
        " (" + task() + ")";
}
