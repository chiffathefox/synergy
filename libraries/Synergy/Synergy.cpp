
#include "Synergy.h"


bool Synergy::sInstance = false;


void Synergy::parse()
{
    char *p;

    while (controlSerial()->available()) {
        char c = controlSerial()->read();

        if (c != 'U') {
            if (debugSerial() != nullptr) {
                debugSerial()->print(c);
            }
        }

        switch (mMessageState) {


        case MessageU:

            if (c == 'U') {
                mMessageState = MessageA;
            } else {
                if (debugSerial() != nullptr) {
                    //debugSerial()->print(c);
                }
            }

            break;


        case MessageA:

            mMessageState = c == 'A' ? MessageR : MessageU;

            break;


        case MessageR:

            mMessageState = c == 'R' ? MessageT : MessageU;

            break;


        case MessageT:

            mMessageState = c == 'T' ? MessageCmd : MessageU;
            mCmdBuffer[0] = 0;

            break;


        case MessageCmd:

            p = strchr(mCmdBuffer, 0);
            *p = c;
            *(p + 1) = 0;

            if (p - mCmdBuffer < 4) {
                break;
            }

            if (strcmp(mCmdBuffer, "jbEnd") == 0) {
                mMessageState = MessageJobFinished;
            } else if (strcmp(mCmdBuffer, "nwJob") == 0) {
                mMessageState = MessageNewJobSpace;
            } else {
                mMessageState = MessageU;
            }

            break;


        case MessageJobFinished:

            if (mode() == Master) {
                mJobState = JobFinished;
            }

            mMessageState = MessageU;

            break;


        case MessageNewJobSpace:

            mMessageState =
                mode() == Slave && c == ' ' ? MessageNewJobTask : MessageU;

            mCmdBuffer[0] = 0;

            break;


        case MessageNewJobTask:

            if (c == '\n') {
                mJobState = JobAvailable;
                mMessageState = MessageU;
                
                break;
            }

            p = strchr(mCmdBuffer, 0);

            if (p >= mCmdBuffer + sizeof (mCmdBuffer) - 2) {
                mMessageState = MessageU;

                break;
            }

            *p = c;
            *(p + 1) = 0;

            break;


        }
    }
}


Synergy::Synergy(Mode mode, const char *ssid, const char *pwd,
        HardwareSerial *controlSerial, HardwareSerial *debugSerial)
    : mSsid(ssid),
    mPwd(pwd),
    mControlSerial(controlSerial),
    mDebugSerial(debugSerial),
    mOk(false),
    mMessageState(MessageU),
    mJobState(JobNone),
    mMode(mode)
{
    if (ssid == nullptr || !*ssid || pwd == nullptr ||
            controlSerial == nullptr || sInstance) {

        return;
    }

    auto n = strlen(pwd);

    if (n < MinPwdLength || n > MaxPwdLength) {
        return;
    }

    mOk = sInstance = true;
}


Synergy::~Synergy()
{
    sInstance = false;
}


void Synergy::begin()
{
    if (!ok()) {
        return;
    }

    controlSerial()->end();
    controlSerial()->begin(115200);

    while (!controlSerial());

    /* TODO: delay? */

    controlSerial()->print("UARTstart ");
    controlSerial()->print(mode() == Master ? "M " : "S ");
    controlSerial()->print(ssid());
    controlSerial()->print(' ');
    controlSerial()->print(pwd());
    controlSerial()->print('\n');
}


void Synergy::broadcastJob(const char *task)
{
    if (!ok() || mode() != Master || strchr(task, '\n') ||
            strlen(task) > MaxTaskLength || mJobState == JobRunning) {

        return;
    }

    mJobState = JobRunning;

    controlSerial()->print("UARTbrdct ");
    controlSerial()->print(task);
    controlSerial()->print('\n');
}


bool Synergy::jobFinished()
{
    if (ok() && mode() == Master) {
        parse();

        return mJobState == JobFinished;
    }

    return false;
}


const char *Synergy::availableJob()
{
    if (ok() && mode() == Slave) {
        parse();

        return mJobState == JobAvailable ? mCmdBuffer : nullptr;
    }

    return nullptr;
}


void Synergy::finishJob()
{
    if (ok() && mode() == Slave) {
        mJobState = JobNone;

        controlSerial()->print("UARTjbEnd\n");
    }
}
