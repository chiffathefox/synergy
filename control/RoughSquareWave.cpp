
#include <Arduino.h>

#include "RoughSquareWave.hpp"


Synergy::RoughSquareWave::RoughSquareWave(int pin, unsigned long timeHigh,
        unsigned long timeLow)
    : mPin(pin),
    mTimeHigh(timeHigh),
    mTimeLow(timeLow),
    mLastTime(0),
    mHigh(false)
{

}


void Synergy::RoughSquareWave::start()
{
    mHigh = false;
    mLastTime = millis();

    pinMode(pin(), OUTPUT);
    digitalWrite(pin(), LOW);
}


void Synergy::RoughSquareWave::stop()
{
    mLastTime = 0;
    digitalWrite(pin(), LOW);
}


void Synergy::RoughSquareWave::loop()
{
    if (mLastTime != 0 &&
            millis() - mLastTime >= (mHigh ? timeHigh() : timeLow())) {

        mHigh = !mHigh;
        mLastTime = millis();
        digitalWrite(pin(), mHigh);
    }
}
