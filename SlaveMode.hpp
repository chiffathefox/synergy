
#pragma once


namespace Synergy
{
    class SlaveMode;
};


class Synergy::SlaveMode
{


public:

    static const uint16_t Port;


    void start(const char *ssid, const char *pwd);
    void stop();

};
