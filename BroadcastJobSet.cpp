
#include "Job.hpp"
#include "BroadcastJobSet.hpp"


Synergy::BroadcastJobSet::BroadcastJobSet(MasterMode *master)
    : mMaster(master)
{

}


Synergy::MasterMode *Synergy::BroadcastJobSet::master() const
{
    return mMaster;
}


void Synergy::BroadcastJobSet::broadcast()
{
    for (auto const &it : master()->slaves()) {
        master()->addJob(Job(it.second, this));
    }
}
