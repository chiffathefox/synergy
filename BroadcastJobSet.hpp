
#pragma once


#include "JobSet.hpp"
#include "MasterMode.hpp"


namespace Synergy
{
    class BroadcastJobSet;
};


class Synergy::BroadcastJobSet : public JobSet
{

    MasterMode *mMaster;


public:

    explicit BroadcastJobSet(MasterMode *master);

    MasterMode *master() const;

    void broadcast();

};
