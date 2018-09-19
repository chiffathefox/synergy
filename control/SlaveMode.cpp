
#include <string>
#include <ESP8266WiFi.h>

#include "Debug.hpp"
#include "MasterMode.hpp"
#include "NewJobMessage.hpp"
#include "JobFinishedMessage.hpp"
#include "JobFinishedAcceptedMessage.hpp"
#include "Slave.hpp"
#include "SlaveBeaconMessage.hpp"
#include "SlaveMode.hpp"


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


void Synergy::SlaveMode::sendJobFinished(Job::id_t jobId)
{
    if (mFinishedJobs.size() < MaxFinishedJobs) {
        mFinishedJobs[jobId] = nullptr;
    } else {
        debugWarn() << "reached finished jobs limit" << mFinishedJobs.size()
                    << "/" << MaxFinishedJobs;
    }

    JobFinishedMessage message;

    message.setJobId(jobId);

    mUdp.beginPacket(masterAddr(), MasterMode::Port);
    mUdp.write(message.raw(), message.rawLength());
    mUdp.endPacket();
}


void Synergy::SlaveMode::parseUdp()
{
    int n = mUdp.parsePacket();

    if (n) {
        auto addrStr = mUdp.remoteIP().toString();
        char buffer[Message::maxRawLength()];

        n = mUdp.read(buffer, sizeof (buffer));

        Message message(buffer, n, Message::Type::None);

        if (!message.ok()) {
            debugWarn() << "received a corrupt message from" << addrStr.c_str();

            return;
        }

        switch (message.type()) {


        default:

            debugLog() << "ignored a" << message.type() << "message from"
                       << addrStr.c_str();

            break;


        case Message::Type::NewJob: {

            NewJobMessage message(buffer, n);

            if (!message.ok()) {
                debugWarn() << "received a corrupt NewJobMessage from"
                            << addrStr.c_str();

                return;
            }

            auto jobId = message.jobId();
            auto task = std::string(message.task(),
                    message.taskLength()).c_str();

            if (jobId < mLastJobId) {
                sendJobFinished(jobId);

                debugWarn() << "received a stale job" << jobId << "with task"
                            << task << "from" << addrStr.c_str();
                debugWarn() << "Last job had an id" << mLastJobId;

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

                debugWarn() << "dropped a new job" << jobId << "with task"
                            << task << "from" << addrStr.c_str();

                sendJobFinished(jobId);

                return;
            }

            setCurrentJob(jobId);

            debugInfo() << "received a new job" << jobId << "with task"
                        << task << "from" << addrStr.c_str();

            Serial.printf("UARTnwJob %s\n", task);

            break;
        }


        case Message::Type::JobFinishedAccepted: {

            JobFinishedAcceptedMessage message(buffer, n);

            if (!message.ok()) {
                debugWarn() << "received a corrupt JobFinishedAcceptedMessage"
                            << "from" << addrStr.c_str();

                return;
            }

            auto jobId = message.jobId();
            auto it = mFinishedJobs.find(jobId);

            if (it == mFinishedJobs.end()) {
                debugLog() << "received JobFinishedAcceptedMessage"
                           << "on an unknown job" << jobId;

                return;
            }

            debugLog() << "received JobFinishedAcceptedMessage for job"
                       << jobId;

            mFinishedJobs.erase(it);

            break;
        }


        }
    }
}


void Synergy::SlaveMode::sendBeacon()
{
    if (millis() - mLastHeartbeat >= Slave::HeartbeatPeriod) {
        SlaveBeaconMessage message;

        mUdp.beginPacket(masterAddr(), MasterMode::Port);
        mUdp.write(message.raw(), message.rawLength());
        mUdp.endPacket();

        mLastHeartbeat = millis();
    }
}


void Synergy::SlaveMode::resendJobFinished()
{
    if (millis() - mLastResendJobFinished >= ResendJobFinishedPeriod) {
        for (auto const &it : mFinishedJobs) {
            sendJobFinished(it.first);
        }

        mLastResendJobFinished = millis();
    }
}


Synergy::SlaveMode::SlaveMode()
    : mRunning(false),
    mLastHeartbeat(0),
    mLastJobId(0),
    mRunningJob(false),
    mLed(LED_BUILTIN, 100, 100),
    mLastResendJobFinished(0)
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

    debugInfo() << "entered slave mode on AP" << ssid << pwd;

    mRunning = true;
    mLastJobId = 0;
    mRunningJob = false;
}


void Synergy::SlaveMode::stop()
{
    if (mRunning) {
        currentJobFinished();

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

    /* 
     * TODO: reset mLastJobId once we connect to a master
     * TODO: for some reason slave stops sending out beacons after
     *       master goes out offline.
     * TODO: reset mUdp on reconnect
     */

    if (!mUdp) {
        mUdp.begin(Port);
    }

    sendBeacon();
    parseUdp();
    resendJobFinished();
}


void Synergy::SlaveMode::currentJobFinished()
{
    if (mRunningJob) {
        mRunningJob = false;

        sendJobFinished(mLastJobId);
    } else {
        Serial.println("Called currentJobFinished, but not jobs running");
    }
}
