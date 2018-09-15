
#include <string>
#include <ESP8266WiFi.h>

#include "MasterMode.hpp"
#include "NewJobMessage.hpp"
#include "JobFinishedMessage.hpp"
#include "Slave.hpp"
#include "SlaveBeaconMessage.hpp"
#include "SlaveMode.hpp"


const uint16_t Synergy::SlaveMode::Port = 1001;


IPAddress Synergy::SlaveMode::masterAddr() const
{
    
    /* Broadcast to the whole network for now. */

    return IPAddress();
}


void Synergy::SlaveMode::setCurrentJob(Job::id_t jobId)
{
    mLastJobId = jobId;
    mRunningJob = true;
}


Synergy::SlaveMode::SlaveMode()
    : mRunning(false),
    mLastHeartbeat(0),
    mLastJobId(0),
    mRunningJob(false),
    mLed(LED_BUILTIN, 100, 100)
{

}


void Synergy::SlaveMode::start(const char *ssid, const char *pwd)
{
    if (mRunning) {
        stop();
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    mLed.start();

    delay(100);

    Serial.printf("Slaved to %s %s\n", ssid, pwd);

    mRunning = true;
}


void Synergy::SlaveMode::stop()
{
    if (mRunning) {
        mUdp.stop();
        mLed.stop();

        if (WiFi.isConnected()) {
            WiFi.disconnect();

            while (WiFi.isConnected()) {
                delay(100);
            }
        }

        mRunning = false;
    }
}


void Synergy::SlaveMode::loop()
{
    mLed.loop();

    if (!mRunning || WiFi.status() != WL_CONNECTED) {
        return;
    }

    if (!mUdp) {
        mUdp.begin(Port);
    }

    if (millis() - mLastHeartbeat >= Slave::HeartbeatPeriod) {
        SlaveBeaconMessage message;

        mUdp.beginPacket(masterAddr(), MasterMode::Port);
        mUdp.write(message.raw(), message.rawLength());
        mUdp.endPacket();

        mLastHeartbeat = millis();
    }

    int n = mUdp.parsePacket();

    if (n) {
        auto addrStr = mUdp.remoteIP().toString();
        char buffer[Message::maxRawLength()];

        n = mUdp.read(buffer, sizeof (buffer));

        Message message(buffer, n, Message::Type::None);

        if (!message.ok()) {
            Serial.printf("Received a corrupt message from %s\n",
                    addrStr.c_str());

            return;
        }

        switch (message.type()) {


        default:

            Serial.printf("Ignored a %u message from %s", message.type(),
                    addrStr.c_str());

            break;


        case Message::Type::NewJob:

            NewJobMessage message(buffer, n);

            if (!message.ok()) {
                Serial.printf("Received a corrupt NewJobMessage from %s\n",
                        addrStr.c_str());
            }

            auto jobId = message.jobId();
            auto task = std::string(message.task(),
                    message.taskLength()).c_str();

            if (jobId < mLastJobId) {
                Serial.printf("Received a stale job #%lu (%s) from %s. "
                        "Last job was %lu\n", (unsigned long) jobId, task,
                        addrStr.c_str(), (unsigned long) mLastJobId);

                return;
            }

            if (jobId == mLastJobId) {

                /* Still running this one */

                return;
            }

            if (mRunningJob) {

                /* 
                 * XXX: still running a job, drop the new one.
                 * XXX: is it better to stack them?
                 */

                Serial.printf("Dropped a job #%lu (%s) from %s.\n",
                        (unsigned long) jobId, task, addrStr.c_str());

                return;
            }

            setCurrentJob(jobId);

            Serial.printf("Received a new job #%lu (%s) from %s.\n",
                    (unsigned long) jobId, task, addrStr.c_str());

            Serial.printf("UARTnwJob %s\n",
                    std::string(message.task(), message.taskLength()).c_str());

            break;


        }
    }
}


void Synergy::SlaveMode::currentJobFinished()
{
    if (mRunningJob) {
        mRunningJob = false;

        JobFinishedMessage message;

        message.setJobId(mLastJobId);

        mUdp.beginPacket(masterAddr(), MasterMode::Port);
        mUdp.write(message.raw(), message.rawLength());
        mUdp.endPacket();
    } else {
        Serial.println("Called currentJobFinished, but not jobs running");
    }
}
