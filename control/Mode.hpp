
#pragma once


namespace Synergy
{
    class Mode;
};


class Synergy::Mode
{


public:

    virtual void start(const char *ssid, const char *pwd) = 0;
    virtual void stop() = 0;
    virtual void loop() = 0;

};
