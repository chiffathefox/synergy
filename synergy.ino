
#include <map>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "MasterMode.hpp"
#include "SlaveMode.hpp"


enum Status {
    STATUS_OK,
    STATUS_DONE
};

enum Mode {
    MODE_IDLE = 0,
    MODE_MASTER,
    MODE_SLAVE
};

typedef Status (*CmdHandler)();


static const int CmdNameLength = 5;
static const int MaxSsidLength = 16;
static const int MaxPwdLength = 16;

static std::map<std::string, CmdHandler> handlers;
static std::map<std::string, unsigned long> stat;

static std::string ssid;
static std::string pwd;
static Mode mode;
static Mode lastMode;

static Synergy::MasterMode master;
static Synergy::SlaveMode slave;


static void start()
{
    switch (lastMode) {


    case MODE_IDLE:

        break;


    case MODE_MASTER:

        master.stop();

        break;


    case MODE_SLAVE:

        slave.stop();

        break;


    }

    switch (mode) {

    
    case MODE_IDLE:

        break;


    case MODE_MASTER:

        master.start(ssid.c_str(), pwd.c_str());

        break;


    case MODE_SLAVE:

        slave.start(ssid.c_str(), pwd.c_str());

        break;


    }
}


static void setMode(Mode newMode)
{
    lastMode = mode;
    mode = newMode;
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
                setMode(MODE_MASTER);
            } else if (c == 'S') {
                setMode(MODE_SLAVE);
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
                start();
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


static void updateStat(const char *key)
{
    Serial.print("Update stat ");
    Serial.println(key);
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
    WiFi.persistent(false);

    delay(1000);
    Serial.begin(115200);
    Serial.println();

    handlers["start"] = &cmdStart;

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    ssid.reserve(MaxSsidLength);
    pwd.reserve(MaxPwdLength);
}


void loop()
{
    parseSerial();
}
