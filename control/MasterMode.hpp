
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

    using SlavesMap = std::map<Slave::id_t, Slave *>;


    static const unsigned int LedHighQuant = 50;


    WiFiUDP mUdp;
    bool mRunning;
    SlavesMap mSlaves;
    std::map<Job::id_t, Job *> mJobs;
    RoughSquareWave mLed;


    void parseUdp();
    void updateJobsHeartbeats();

    Slave *createSlave(const IPAddress &addr);
    SlavesMap::iterator deleteSlave(SlavesMap::iterator &it);

    void uartSendJobFinished(Job *job);

    void sendJobFinishedAccepted(Job::id_t jobId);


public:

    static const uint16_t Port = 1001;
    static const size_t MaxSlaves = 8;
    static const size_t MaxJobs = 64;


    MasterMode();
    ~MasterMode();

    std::map<Slave::id_t, Slave *> slaves() const;

    void sendJob(Job *job);
    void registerJob(Job *job);
    void jobFinished(Job *job);

    virtual void start(const char *ssid, const char *pwd) override;
    virtual void stop() override;

    virtual void loop() override;

};
