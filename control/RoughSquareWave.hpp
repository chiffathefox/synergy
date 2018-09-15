
#pragma once


namespace Synergy
{
    class RoughSquareWave;
};


class Synergy::RoughSquareWave
{

    int mPin;
    unsigned long mTimeHigh;
    unsigned long mTimeLow;
    unsigned long mLastTime;
    bool mHigh;


public:

    explicit RoughSquareWave(int pin,
            unsigned long timeHigh, unsigned long timeLow);

    void start();
    void stop();
    void loop();


    inline int pin() const
    {
        return mPin;
    }


    inline unsigned long timeHigh() const
    {
        return mTimeHigh;
    }


    inline void setTimeHigh(unsigned long timeHigh)
    {
        mTimeHigh = timeHigh;
    }


    inline unsigned long timeLow() const
    {
        return mTimeLow;
    }


    inline void setTimeLow(unsigned long timeLow)
    {
        mTimeLow = timeLow;
    }


};
