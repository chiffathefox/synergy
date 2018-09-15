
#include <map>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "Debug.hpp"
#include "Job.hpp"
#include "MasterMode.hpp"
#include "SlaveMode.hpp"
#include "BroadcastJob.hpp"
#include "RoughSquareWave.hpp"


enum Status {
    STATUS_OK,
    STATUS_DONE
};


typedef Status (*CmdHandler)();


static const int CmdNameLength = 5;
static const int MaxSsidLength = 16;
static const int MaxPwdLength = 16;

static std::map<std::string, CmdHandler> handlers;
static std::map<std::string, unsigned long> stat;

static std::string ssid;
static std::string pwd;
static std::string cmdBuffer;

static Synergy::Mode *mode = nullptr;
static Synergy::MasterMode master;
static Synergy::SlaveMode slave;
static Synergy::RoughSquareWave led(LED_BUILTIN, 1000, 50);


static void setMode(Synergy::Mode *newMode, const char *ssid, const char *pwd)
{
    if (mode != nullptr) {
        mode->stop();
    }

    mode = newMode;

    led.stop();
    mode->start(ssid, pwd);
}


static Status cmdStart()
{
    static enum {
        CMD_START_SPACE = 0,
        CMD_START_MODE,
        CMD_START_MODE_SPACE,
        CMD_START_SSID,
        CMD_START_PWD
    } cmdState;

    static Synergy::Mode *cmdMode;

    while (Serial.available()) {
        char c = Serial.read();

        switch (cmdState) {

        
        case CMD_START_SPACE:

            if (c != ' ') {
                updateStat("CMD_START_NO_FIRST_SPACE");
                cmdState = CMD_START_SPACE;

                return STATUS_DONE;
            }

            ssid.clear();
            pwd.clear();

            cmdState = CMD_START_MODE;

            break;


        case CMD_START_MODE:

            if (c == 'M') {
                cmdMode = &master;
            } else if (c == 'S') {
                cmdMode = &slave;
            } else {
                updateStat("CMD_START_WRONG_MODE");
                cmdState = CMD_START_SPACE;

                return STATUS_DONE;
            }

            cmdState = CMD_START_MODE_SPACE;

            break;


        case CMD_START_MODE_SPACE:

            if (c != ' ') {
                updateStat("CMD_START_NO_MODE_SPACE");
                cmdState = CMD_START_SPACE;

                return STATUS_DONE;
            }

            cmdState = CMD_START_SSID;

            break;


        case CMD_START_SSID:

            if (c == ' ') {
                cmdState = CMD_START_PWD;
                
                break;
            } else if (c == '\n') {
                updateStat("CMD_START_NO_PWD");
                cmdState = CMD_START_SPACE;

                return STATUS_DONE;
            } else if (ssid.length() >= MaxSsidLength) {
                updateStat("CMD_START_SSID_TOO_LONG");
                cmdState = CMD_START_SPACE;

                return STATUS_DONE;
            }

            ssid += c;

            break;


        case CMD_START_PWD:

            if (c == ' ') {
                updateStat("CMD_START_SPACE_IN_PWD");
            } else if (c == '\n') {
                setMode(cmdMode, ssid.c_str(), pwd.c_str());
            } else if (pwd.length() >= MaxPwdLength) {
                updateStat("CMD_START_PWD_TOO_LONG");
            } else {
                pwd += c;

                break;
            }

            cmdState = CMD_START_SPACE;

            return STATUS_DONE;


        }
    }

    return STATUS_OK;
}


static Status cmdBroadcast()
{
    static enum {
        CmdBroadcastFirstSpace = 0,
        CmdBroadcastTask
    } state;

    if (mode != &master) {
        updateStat("CMD_BROADCAST_NOT_A_MASTER");

        return STATUS_DONE;
    }


    while (Serial.available()) {
        char c = Serial.read();

        switch (state) {


        case CmdBroadcastFirstSpace:

            if (c != ' ') {
                updateStat("CMD_BROADCAST_NO_FIRST_SPACE");

                return STATUS_DONE;
            }

            cmdBuffer.clear();
            state = CmdBroadcastTask;

            break;


        case CmdBroadcastTask:

            if (c != '\n') {
                if (cmdBuffer.length() > Synergy::Job::MaxTaskLength) {
                    state = CmdBroadcastFirstSpace;

                    updateStat("CMD_BROADCAST_TASK_TOO_LONG");

                    return STATUS_DONE;
                }

                cmdBuffer += c;

                return STATUS_OK;
            }

            /* `job` gets deleted by `MasterMode::jobFinished` */

            auto job = new Synergy::BroadcastJob(&master,
                    cmdBuffer.c_str(), cmdBuffer.length());

            job->emit();
            state = CmdBroadcastFirstSpace;

            Debugf("broadcasted a new job #%u\n", job->id());

            return STATUS_DONE;


        }
    }

    return STATUS_OK;
}


static Status cmdJobFinished()
{
    if (mode != &slave) {
        updateStat("CMD_JOB_FINISHED_NOT_A_SLAVE");
    } else {
        slave.currentJobFinished();
    }

    return STATUS_DONE;
}


static void updateStat(const char *key)
{
    Debugf("Update stat %s\n", key);
    stat[key]++;
}


static void parseSerial()
{
    static enum {
        FRAME_START_U = 0,
        FRAME_START_A,
        FRAME_START_R,
        FRAME_START_T,
        FRAME_STARTED,
        FRAME_HANDLER
    } frameState;

    static CmdHandler currentHandler;

    while (Serial.available()) {
        char c = 0;

        if (frameState < FRAME_STARTED) {
            c = Serial.read();
        }

        switch (frameState) {


        case FRAME_START_U:

            if (c == 'U') {
                frameState = FRAME_START_A;
            }

            continue;


        case FRAME_START_A:

            frameState = c == 'A' ? FRAME_START_R : FRAME_START_U;

            continue;


        case FRAME_START_R:

            frameState = c == 'R' ? FRAME_START_T : FRAME_START_U;

            continue;


        case FRAME_START_T:

            frameState = c == 'T' ? FRAME_STARTED : FRAME_START_U;

            continue;


        case FRAME_STARTED:

            if (Serial.available() < CmdNameLength) {
                return;
            }

            break;


        case FRAME_HANDLER:

            if (currentHandler() == STATUS_DONE) {
                frameState = FRAME_START_U;
            }

            return;


        }

        std::string buf;

        buf.reserve(CmdNameLength);
        buf.resize(CmdNameLength, 0);
        Serial.readBytes(&buf[0], CmdNameLength);

        if (handlers.find(buf) == handlers.end()) {
            updateStat("SERIAL_WRONG_CMD_NAME");
            frameState = FRAME_START_U;
        } else {
            frameState = FRAME_HANDLER;
            currentHandler = handlers[buf];

            if (currentHandler() == STATUS_DONE) {
                frameState = FRAME_START_U;
            }
        }
    }
}


void setup()
{
    led.start();
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);

    delay(1000);
    Serial.begin(115200);

    handlers["start"] = &cmdStart;
    handlers["brdct"] = &cmdBroadcast;
    handlers["jbEnd"] = &cmdJobFinished;

    ssid.reserve(MaxSsidLength);
    pwd.reserve(MaxPwdLength);
    cmdBuffer.reserve(255);
}


void loop()
{
    parseSerial();

    if (mode != nullptr) {
        mode->loop();
    }

    led.loop();
}
