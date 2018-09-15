
#include <ESP8266WiFi.h>

#include "Message.hpp"
#include "NewJobMessage.hpp"
#include "JobFinishedMessage.hpp"
#include "SlaveMode.hpp"
#include "MasterMode.hpp"


const uint16_t Synergy::MasterMode::Port = 1001;


Synergy::MasterMode::MasterMode()
    : mRunning(false)
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
    delete job;
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

    mRunning = true;

    Serial.printf("brough up %s %s\n", ssid, pwd);
}


void Synergy::MasterMode::stop() {
    if (!mRunning) {
        mUdp.stop();
        WiFi.mode(WIFI_STA);

        mRunning = false;

        delay(100);
    }
}


void Synergy::MasterMode::loop()
{
    if (mRunning) {
        int n = mUdp.parsePacket();

        if (n) {
            IPAddress addr = mUdp.remoteIP();

            Serial.printf("Received %d bytes from %s", n,
                    addr.toString().c_str());

            if ((static_cast<uint32_t>(addr) ^ WiFi.localIP()) & 0xFFFFFF00) {
                
                Serial.println("Ignored a foreign IP");

                return;
            }

            Slave *slave = mSlaves[addr[3]];

            if (slave == nullptr) {
                slave = new Slave(addr);
                mSlaves[slave->id()] = slave;

                Serial.println("Added a new slave");
            }

            slave->updateHeartbeat();

            char buffer[255];

            n = mUdp.read(buffer, sizeof (buffer));

            Message message(buffer, n, Message::Type::None);

            if (!message.ok()) {
                return;
            }


            switch (message.type()) {


            case Message::Type::Beacon:
            case Message::Type::NewJob:

                return;


            case Message::Type::JobFinished:

                JobFinishedMessage message(buffer, n);

                if (!message.ok()) {
                    Serial.println("Received a corrupt JonFinishedMessage");

                    return;
                }

                auto jobId = message.jobId();

                if (mJobs.find(jobId) == mJobs.end()) {
                    Serial.printf("Received a stale job %llu\n", jobId);

                    return;
                }

                mJobs[jobId]->finished(jobId, slave);

                break;


            }
        }
    }
}
