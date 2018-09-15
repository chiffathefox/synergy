
#include <ESP8266WiFi.h>

#include "SlaveMode.hpp"


const uint16_t Synergy::SlaveMode::Port = 1001;


void Synergy::SlaveMode::start(const char *ssid, const char *pwd)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);

    delay(100);

    Serial.printf("Slaved to %s %s\n", ssid, pwd);
}


void Synergy::SlaveMode::stop()
{
    if (WiFi.isConnected()) {
        WiFi.disconnect();

        while (WiFi.isConnected()) {
            delay(100);
        }
    }
}
