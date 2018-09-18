
#pragma once


#include <cstdint>
#include <WiFiUdp.h>

#include "Job.hpp"
#include "Mode.hpp"
#include "RoughSquareWave.hpp"


namespace Synergy
{
    class SlaveMode;
};


class Synergy::SlaveMode : public Mode
{

    WiFiUDP mUdp;
    bool mRunning;
    unsigned long mLastHeartbeat;
    Job::id_t mLastJobId;
    bool mRunningJob;
    RoughSquareWave mLed;


    IPAddress masterAddr() const;
    void setCurrentJob(Job::id_t jobId);
    void sendJobFinished(Job::id_t jobId);


public:

    static const uint16_t Port = 1001;


    SlaveMode();

    virtual void start(const char *ssid, const char *pwd) override;
    virtual void stop() override;
    virtual void loop() override;

    void currentJobFinished();

};
