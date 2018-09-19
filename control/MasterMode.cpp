
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

    if (n <= 0) {
        return;
    }

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

    Slave *slave;
    auto it = mSlaves.find(slaveId);

    if (it == mSlaves.end()) {
        if (message.type() == Message::Type::SlaveBeacon) {
            slave = createSlave(addr);

            if (slave == nullptr) {
                return;
            }
        } else {
            debugWarn() << "received a vailid message from an unknown"
                << "slave" << slaveId;

            return;
        }
    } else {
        slave = it->second;
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


void Synergy::MasterMode::updateJobsHeartbeats()
{
    for (auto const &it : mJobs) {
        auto job = it.second;

        if (millis() - job->heartbeat() >= Job::HeartbeatPeriod) {
            job->updateHeartbeat();
        }
    }
}


Synergy::Slave *Synergy::MasterMode::createSlave(const IPAddress &addr)
{
    if (mSlaves.size() >= MaxSlaves) {
        debugWarn() << "reached slaves limit. ("
                    << mSlaves.size() << "/" << MaxSlaves
                    << "). Attempting to delete some ...";

        for (auto it = mSlaves.begin(); it != mSlaves.end(); it++) {
            if (!it->second->alive()) {
                deleteSlave(it);
                it = mSlaves.erase(it);

                break;
            }
        }

        if (mSlaves.size() >= MaxSlaves) {
            debugEmerg() << "not enough slaves were removed, the new slave"
                         << "is ignored";

            return nullptr;
        }
    }

    auto slave = new Slave(addr);

    debugInfo() << "add slave" << slave->id() << "@"
                << slave->addr().toString().c_str();

    mSlaves[slave->id()] = slave;
    mLed.setTimeHigh(LedHighQuant * mSlaves.size());

    return slave;
}


Synergy::MasterMode::SlavesMap::iterator
Synergy::MasterMode::deleteSlave(SlavesMap::iterator &it)
{
    auto slave = it->second;

    debugInfo() << "delete slave" << slave->id() << "@"
                << slave->addr().toString().c_str();

    mLed.setTimeHigh(LedHighQuant * (mSlaves.size() - 1));

    delete slave;

    return mSlaves.erase(it);
}


void Synergy::MasterMode::uartSendJobFinished(Job *job)
{
    Serial.print("UARTjbEnd\n");
    debugInfo() << job->toString() << "was finished";
}


Synergy::MasterMode::MasterMode()
    : mRunning(false),
    mLed(LED_BUILTIN, 0, 500)
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


void Synergy::MasterMode::sendJob(Job *job)
{
    auto slave = job->slave();
    auto message = job->message();

    mUdp.beginPacket(slave->addr(), SlaveMode::Port);
    mUdp.write(message.raw(), message.rawLength());
    mUdp.endPacket();
}


void Synergy::MasterMode::registerJob(Job *job)
{
    if (mJobs.size() >= MaxJobs) {
        debugEmerg() << "reached jobs limit, dropping the new"
                     << job->toString();

        uartSendJobFinished(job);

        delete job;

        return;
    }

    if (mJobs.find(job->id()) == mJobs.end()) {
        mJobs[job->id()] = job;
        job->emit();
    } else {
        debugEmerg() << "tried to register an already registered"
                     << job->toString()
                     << (job == mJobs[job->id()] ?
                         "which is the same as the one alredy registered" :
                         "which is NOT the same as the one already registered");
    }
}


void Synergy::MasterMode::jobFinished(Job *job)
{
    auto it = mJobs.find(job->id());

    if (it == mJobs.end()) {
        debugEmerg() << "called on an unknown to us" << job->toString();
    } else {
        uartSendJobFinished(job);
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
