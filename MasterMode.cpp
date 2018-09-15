
#include <ESP8266WiFi.h>

#include "Message.hpp"
#include "NewJobMessage.hpp"
#include "JobFinishedMessage.hpp"
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


std::map<uint8_t, Synergy::Slave *> Synergy::MasterMode::slaves() const
{
    return mSlaves;
}


void Synergy::MasterMode::addJob(const Job &job)
{
    mJobs[job.id()] = job;

    mUdp.beginPacket(job.addr(), SlaveMode::Port);

    NewJobMessage message(nullptr, -1);

    message.setJobId(job.id());

    mUdp.write(message.raw(), );
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
                mSlaves[addr[3]] = slave;

                Serial.println("Added a new slave");
            }

            slave->updateHeartbeat();

            char buffer[255];

            n = mUdp.read(buffer, sizeof (buffer));

            Message message(buffer, n, Message::Type::Beacon);

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

                if (mJobs.find(message.jobId()) == mJobs.end()) {
                    Serial.printf("Received a stale job %llu\n",
                            message.jobId());

                    return;
                }

                mJobs[message.jobId()].finished();

                break;


            }
        }
    }
}
