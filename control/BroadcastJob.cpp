
#include "BroadcastJob.hpp"


Synergy::BroadcastJob::BroadcastJob(MasterMode *master, const char *task,
        uint8_t taskLength)
    : Job(master, task, taskLength)
{
    mMessage.setTask(task, taskLength);
}


void Synergy::BroadcastJob::updateHeartbeat()
{
    Job::updateHeartbeat();

    for (auto slave : mSlaves) {
        setSlave(slave);
        mMessage.setJobId(id());
        mMessage.setTask(task(), taskLength());
        master()->emitJob(this);
    }

    setSlave(nullptr);
    mMessage.setJobId(0);
}


void Synergy::BroadcastJob::emit()
{
    if (!mSlaves.empty()) {
        return;
    }

    for (auto const &it : master()->slaves()) {
        auto slave = it.second;

        if (slave->alive()) {
            mSlaves.push_back(slave);
        }
    }

    updateHeartbeat();
}


Synergy::JobMessage Synergy::BroadcastJob::message() const
{
    return mMessage;
}


void Synergy::BroadcastJob::finished(Slave *slave)
{
    for (auto it = mSlaves.begin(); it != mSlaves.end(); it++) {
        if (*it == slave) {
            mSlaves.erase(it);

            if (mSlaves.empty()) {
                master()->jobFinished(this);

                /* No class fields may be accessed past this point! */

            }

            break;
        }
    }
}
