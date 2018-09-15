
#include <ESP8266WiFi.h>

#include "Message.hpp"
#include "NewJobMessage.hpp"
#include "JobFinishedMessage.hpp"
#include "SlaveMode.hpp"
#include "MasterMode.hpp"


const uint16_t Synergy::MasterMode::Port = 1001;


Synergy::MasterMode::MasterMode()
    : mRunning(false),
    mLed(LED_BUILTIN, 50, 500)
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
    Serial.println("UARTjbEnd");

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
    mLed.start();

    mRunning = true;

    Serial.printf("brough up %s %s\n", ssid, pwd);
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
    if (mRunning) {
        mLed.loop();

        int n = mUdp.parsePacket();

        if (n) {
            IPAddress addr = mUdp.remoteIP();
            Slave::id_t slaveId = Slave(addr).id();

            if ((static_cast<uint32_t>(addr) ^ WiFi.softAPIP()) & 0x00FFFFFF) {
                
                Serial.printf("Ignored a packet from an IP from a foreign " 
                        "subnet (%s)\n", addr.toString().c_str());

                return;
            }

            char buffer[Message::maxRawLength()];

            n = mUdp.read(buffer, sizeof (buffer));

            Message message(buffer, n, Message::Type::None);

            if (!message.ok()) {
                Serial.printf("Received a corrupt message from #%u\n", slaveId);

                return;
            }

            Slave *slave = mSlaves[slaveId];

            if (slave == nullptr) {
                if (message.type() == Message::Type::SlaveBeacon) {
                    slave = new Slave(addr);
                    mSlaves[slaveId] = slave;

                    Serial.printf("Added a new slave #%u\n", slaveId);
                } else {
                    Serial.printf("Received a valid message from an unknown "
                            "slave #%u\n", slaveId);

                    return;
                }
            }

            slave->updateHeartbeat();

            switch (message.type()) {


            default:

                Serial.printf("Ignored a %u message from #%u\n",
                        message.type(), slaveId);

                break;


            case Message::Type::SlaveBeacon:

                break;


            case Message::Type::JobFinished:

                JobFinishedMessage message(buffer, n);

                if (!message.ok()) {
                    Serial.printf("Received a corrupt JobFinishedMessage " 
                            "from #%u\n", slaveId);

                    return;
                }

                auto jobId = message.jobId();

                if (mJobs.find(jobId) == mJobs.end()) {
                    Serial.printf("Received a stale job %lu\n",
                            (unsigned long) jobId);

                    return;
                }

                mJobs[jobId]->finished(slave);

                Serial.printf("#%u has finished job #%lu\n", slaveId,
                        (unsigned long) jobId);

                break;


            }
        }
    }
}
