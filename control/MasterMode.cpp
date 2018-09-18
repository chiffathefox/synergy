
#include <ESP8266WiFi.h>

#include "Debug.hpp"
#include "Message.hpp"
#include "NewJobMessage.hpp"
#include "JobFinishedMessage.hpp"
#include "SlaveMode.hpp"
#include "MasterMode.hpp"


void Synergy::MasterMode::parseUdp()
{
    int n = mUdp.parsePacket();

    if (n) {
        IPAddress addr = mUdp.remoteIP();
        Slave::id_t slaveId = Slave(addr).id();

        if ((static_cast<uint32_t>(addr) ^ WiFi.softAPIP()) & 0x00FFFFFF) {

            debugLog() << "ignored a packet from an IP from a foreign subnet"
                       << addr.toString().c_str();

            return;
        }

        char buffer[Message::maxRawLength()];

        n = mUdp.read(buffer, sizeof (buffer));

        Message message(buffer, n, Message::Type::None);

        if (!message.ok()) {
            debugWarn() << "received a corrupt message from slave" << slaveId;

            return;
        }

        Slave *slave = mSlaves[slaveId];

        if (slave == nullptr) {
            if (message.type() == Message::Type::SlaveBeacon) {
                slave = new Slave(addr);

                addSlave(slave);
            } else {
                debugWarn() << "received a vailid message from an unknown"
                            << "slave" << slaveId;

                return;
            }
        }

        slave->updateHeartbeat();

        switch (message.type()) {


            default:

                debugLog() << "ignored a" << message.type()
                           << "message from slave" << slaveId;

                break;


            case Message::Type::SlaveBeacon:

                break;


            case Message::Type::JobFinished:

                JobFinishedMessage message(buffer, n);

                if (!message.ok()) {
                    debugWarn() << "received a corrupt JobFinishedMessage from"
                                << "slave" << slaveId;

                    return;
                }

                auto jobId = message.jobId();

                if (mJobs.find(jobId) == mJobs.end()) {
                    debugWarn() << "received a JobFinishedMessage on a"
                                << "stale job" << jobId;

                    return;
                }

                debugInfo() << "slave" << slaveId << "has finished job"
                            << jobId;

                mJobs[jobId]->finished(slave);

                break;


        }
    }

}


void Synergy::MasterMode::addSlave(Slave *slave)
{
    debugInfo() << "add slave" << slave->id() << "@"
                << slave->addr().toString().c_str();

    mSlaves[slave->id()] = slave;
    mLed.setTimeHigh(LedHighQuant * (1 + mSlaves.size()));
}


void Synergy::MasterMode::updateJobsHeartbeats()
{
    for (auto const &it : mJobs) {
        auto job = it.second;

        if (millis() - job->heartbeat() >= Job::HeartbeatPeriod) {
            job->updateHeartbeat();
        }
    }
}


Synergy::MasterMode::MasterMode()
    : mRunning(false),
    mLed(LED_BUILTIN, LedHighQuant, 500)
{

}


Synergy::MasterMode::~MasterMode()
{
    stop();
}


std::map<Synergy::Slave::id_t, Synergy::Slave *>
Synergy::MasterMode::slaves() const
{
    return mSlaves;
}


void Synergy::MasterMode::emitJob(Job *job)
{
    mJobs[job->id()] = job;

    auto slave = job->slave();
    auto message = job->message();

    mUdp.beginPacket(slave->addr(), SlaveMode::Port);
    mUdp.write(message.raw(), message.rawLength());
    mUdp.endPacket();
}


void Synergy::MasterMode::jobFinished(Job *job)
{
    auto it = mJobs.find(job->id());

    if (it == mJobs.end()) {
        debugEmerg() << "called on an unknown to us job" << job->id();
    } else {
        Serial.print("UARTjbEnd\n");
        debugInfo() << "job" << job->id() << "was finished";

        mJobs.erase(it);
        delete job;
    }
}


void Synergy::MasterMode::start(const char *ssid, const char *pwd)
{
    if (mRunning) {
        stop();
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pwd);

    delay(100);

    mUdp.begin(Port);
    mLed.start();

    mRunning = true;

    debugInfo() << "entered master mode and brought up AP" << ssid << pwd;
}


void Synergy::MasterMode::stop() {
    if (mRunning) {
        mUdp.stop();
        mLed.stop();
        WiFi.mode(WIFI_STA);

        mRunning = false;

        delay(100);
    }
}


void Synergy::MasterMode::loop()
{

    /* TODO: iterate over jobs in order to implement heartbeat. */
    /* TODO: send a response once a job finishes */
    /* TODO: repeat JobFinishedMessage on slaves */
    /* TODO: job timeout */
    /* TODO: cookies */

    if (mRunning) {
        mLed.loop();
        parseUdp();
        updateJobsHeartbeats();
    }
}
