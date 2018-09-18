
#include <Arduino.h>

#include "Debug.hpp"


void Debug::assertPanic()
{
    pinMode(LED_BUILTIN, OUTPUT);

    for (bool state = LOW ;; state = !state) {
        digitalWrite(LED_BUILTIN, state);
        delay(500);
    }
}


void Debug::assertPanic(const char *file, int line, const char *message)
{
    pinMode(LED_BUILTIN, OUTPUT);

    for (bool state = LOW ;; state = !state) {
        debugEmerg() << "PANIC:" << file << ":" << line << ":" << message;

        digitalWrite(LED_BUILTIN, state);
        delay(500);
    }
}


const char *Debug::stripModifiers(const char *name)
{
    const char *lastSpace = name;

    for (; *name; name++) {
        switch (*name) {


        case ' ':

            lastSpace = name;

            break;


        case '(':

            return lastSpace + 1;


        }
    }

    return name;
}
