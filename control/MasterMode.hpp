
#pragma once


#include <map>
#include <WiFiUdp.h>

#include "Slave.hpp"
#include "Job.hpp"
#include "Mode.hpp"
#include "RoughSquareWave.hpp"


namespace Synergy
{
    class MasterMode;
};


class Synergy::MasterMode : public Mode
{

    WiFiUDP mUdp;
    bool mRunning;
    std::map<Slave::id_t, Slave *> mSlaves;
    std::map<Job::id_t, Job *> mJobs;
    RoughSquareWave mLed;


public:

    static const uint16_t Port;


    MasterMode();
    ~MasterMode();

    std::map<Slave::id_t, Slave *> slaves() const;

    void emitJob(Job *job);
    void jobFinished(Job *job);

    virtual void start(const char *ssid, const char *pwd) override;
    virtual void stop() override;

    virtual void loop() override;

};
