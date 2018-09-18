
#include "Debug.hpp"
#include "BroadcastJob.hpp"


void Synergy::BroadcastJob::propagate()
{
    if (mSlaves.empty()) {
        master()->jobFinished(this);

        /* No class fields may be accessed past this point! */

        return;
    }

    NewJobMessage message;

    mMessage = &message;
    message.setJobId(id());
    message.setTask(task(), taskLength());

    for (auto const &it : mSlaves) {
        setSlave(it.second);
        master()->emitJob(this);
    }

    setSlave(nullptr);
    mMessage = nullptr;
}


Synergy::BroadcastJob::BroadcastJob(MasterMode *master, const char *task,
        uint8_t taskLength)
    : Job(master, task, taskLength),
    mMessage(nullptr)
{

}


void Synergy::BroadcastJob::updateHeartbeat()
{
    Job::updateHeartbeat();

    if (finishOnSlavesDeath()) {
        for (auto it = mSlaves.begin(); it != mSlaves.end(); ) {
            if (!it->second->alive()) {
                mSlaves.erase(it);
            } else {
                it++;
            }
        }
    }

    auto end = mSlaves.end();

    for (auto const &it : master()->slaves()) {
        if (mSlaves.find(it.first) == end && it.second->alive()) {
            mSlaves[it.first] = it.second;
        }
    }

    propagate();
}


void Synergy::BroadcastJob::emit()
{
    if (!mSlaves.empty()) {
        return;
    }

    Job::emit();

    for (auto const &it : master()->slaves()) {
        auto slave = it.second;

        if (slave->alive()) {
            mSlaves[slave->id()] = slave;
        }
    }

    propagate();
}


Synergy::JobMessage Synergy::BroadcastJob::message() const
{
    return *mMessage;
}


void Synergy::BroadcastJob::finished(Slave *slave)
{
    auto it = mSlaves.find(slave->id());

    if (it == mSlaves.end()) {
        debugEmerg() << "called with an unknown to us slave" << slave->id();

        return;
    }

    mSlaves.erase(it);

    if (mSlaves.empty()) {
        master()->jobFinished(this);

        /* No class fields may be accessed past this point! */

    }
}
