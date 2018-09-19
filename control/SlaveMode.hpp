
#pragma once


#include <cstdint>
#include <map>
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

    std::map<Job::id_t, std::nullptr_t> mFinishedJobs;
    unsigned long mLastResendJobFinished;


    IPAddress masterAddr() const;
    void setCurrentJob(Job::id_t jobId);
    void sendJobFinished(Job::id_t jobId);
    void parseUdp();
    void sendBeacon();
    void resendJobFinished();


public:

    static const uint16_t Port = 1001;
    static const size_t MaxFinishedJobs = 64;
    static const unsigned long ResendJobFinishedPeriod = 100;


    SlaveMode();

    virtual void start(const char *ssid, const char *pwd) override;
    virtual void stop() override;
    virtual void loop() override;

    void currentJobFinished();

};
