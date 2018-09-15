
#pragma once


#include "Arduino.h"


/* TODO: add cookies */


class Synergy
{

    static bool sInstance;

    const char *mSsid;
    const char *mPwd;
    HardwareSerial *mControlSerial;
    HardwareSerial *mDebugSerial;
    bool mOk;

    enum {
        MessageU = 0,
        MessageA,
        MessageR,
        MessageT,
        MessageCmd,
        MessageJobFinished,
        MessageNewJobSpace,
        MessageNewJobTask
    } mMessageState;

    enum {
        JobNone = 0,
        JobRunning,
        JobFinished,
        JobAvailable
    } mJobState;

    void parse();
    

public:


    enum Mode
    {
        Master = 0,
        Slave
    };


    static const size_t MinPwdLength = 8;
    static const size_t MaxPwdLength = 31;
    static const size_t MaxTaskLength = 32;


    explicit Synergy(Mode mode, const char *ssid, const char *pwd,
            HardwareSerial *controlSerial = &Serial,
            HardwareSerial *debugSerial = &Serial);
    ~Synergy();
    
    void begin();

    void broadcastJob(const char *task);
    bool jobFinished();

    const char *availableJob();
    void finishJob();
    

    inline Mode mode() const
    {
        return mMode;
    }


    inline const char *ssid() const
    {
        return mSsid;
    }


    inline const char *pwd() const
    {
        return mPwd;
    }


    inline HardwareSerial *controlSerial() const
    {
        return mControlSerial;
    }


    inline HardwareSerial *debugSerial() const
    {
        return mDebugSerial;
    }


    /**
     * @returns false if one of the parameters passed to the contructor was
     *          incorrect.
     */

    inline bool ok() const
    {
        return mOk;
    }


private:

    Mode mMode;
    char mCmdBuffer[MaxTaskLength + 1];

};
