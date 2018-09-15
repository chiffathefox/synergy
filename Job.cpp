
#include "Job.hpp"


uint64_t Synergy::Job::sLastId = 0;


Synergy::Job::Job(MasterMode *master, const char *task, uint8_t taskLength)
    : mMaster(master),
    mTask(task),
    mTaskLength(taskLength),
    mSlave(nullptr),
    mId(getId()),
    mHeartbeat(0)
{

}


Synergy::Job::~Job()
{

}


void Synergy::Job::updateHeartbeat()
{
    mHeartbeat = millis();
}
