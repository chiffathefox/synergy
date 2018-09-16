
#include "Synergy.h"


static Synergy synergy(Synergy::Master, "BAD_DALEK", "thereisnospoon");


void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    synergy.begin();
}


void loop()
{
    static enum {
        EmitFirstJob = 0,
        WaitFirstJob,
        WaitSecondJob
    } state;

    switch (state) {


    case EmitFirstJob:

        if (millis() >= 10000) {
            digitalWrite(LED_BUILTIN, HIGH);

            synergy.broadcastJob("forward");
            state = WaitFirstJob;
        }

        break;


    case WaitFirstJob:

        if (synergy.jobFinished()) {
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(800);

            synergy.broadcastJob("stop");

            state = WaitSecondJob;
        }

        break;

    case WaitSecondJob:

        if (synergy.jobFinished()) {
            digitalWrite(LED_BUILTIN, LOW);
            delay(800);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(50);
        }

        break;


    }
}
