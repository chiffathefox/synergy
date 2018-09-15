
#pragma once


#include <map>
#include <WiFiUdp.h>

#include "Slave.hpp"
#include "Job.hpp"


namespace Synergy
{
    class MasterMode;
};


class Synergy::MasterMode
{

    WiFiUDP mUdp;
    bool mRunning;
    std::map<uint8_t, Slave *> mSlaves;
    std::map<uint64_t, Job> mJobs;


public:

    static const uint16_t Port;


    MasterMode();
    ~MasterMode();

    std::map<uint8_t, Slave *> slaves() const;

    void addJob(const Job &job);

    void start(const char *ssid, const char *pwd);
    void stop();

    void loop();

};
