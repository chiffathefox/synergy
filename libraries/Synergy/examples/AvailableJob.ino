
#include "Synergy.h"


static Synergy synergy(Synergy::Slave, "BAD_DALEK", "thereisnospoon");


void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    synergy.begin();
}


void loop()
{
    const char *task = synergy.availableJob();

    if (task == nullptr) {

        /* Sitting idle */

        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(5);
    } else if (strcmp(task, "forward") == 0) {

        /* ONWARDS! */

        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        synergy.finishJob();
    } else if (strcmp(task, "stop") == 0) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
        synergy.finishJob();
    } else {
        
        /* Unknown jobs should be finished right away */

        Serial.println("Received an unknown job");

        synergy.finishJob();
    }
}
