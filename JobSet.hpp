
#pragma once


namespace Synergy
{
    class Job;
    class JobSet;
};


class Synergy::JobSet
{


public:

    explicit JobSet();

    virtual void finished(const Job &job) = 0;

};
