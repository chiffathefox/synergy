
#include "MasterMode.cpp"


void Synergy::MasterMode::start()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), pwd.c_str());

    delay(100);

    Serial.printf("brough up %s:%s", ssid.c_str(), pwd.c_str());
}
