
#pragma once


#include <Arduino.h>


#ifdef SYNERGY_DEBUG

#    define Debugf(...) Serial.printf(__VA_ARGS__)

#else

#    define Debugf(...)

#endif
