
#pragma once


#define DEBUG 1


#if (DEBUG)


#    include "SerialLogger.hpp"


#define DEBUG_PRETTY_FUNCTION                                           \
    ::Debug::stripModifiers(__PRETTY_FUNCTION__)


#    define debugAssert(condition)                                      \
    if (!(condition)) {                                                 \
        ::Debug::assertPanic(__FILE__, __LINE__, #condition);    \
    }


#    define debugLog()                                                  \
    ::Debug::SerialLogger("D", DEBUG_PRETTY_FUNCTION, "\n", true)


#    define debugWarn()                                                 \
    ::Debug::SerialLogger("W", DEBUG_PRETTY_FUNCTION, "\n", true)


#    define debugEmerg()                                                \
    ::Debug::SerialLogger("E", DEBUG_PRETTY_FUNCTION, "\n", true)


#    define debugInfo()                                                 \
    ::Debug::SerialLogger("I", DEBUG_PRETTY_FUNCTION, "\n", true)


#else


#    include "DummyLogger.hpp"


#    define debugAssert(condition)                                      \
    if (!(condition)) {                                                 \
        ::Debug::assertPanic();                                  \
    }


#    define debugLog()                                                  \
    ::Debug::DummyLogger()


#    define debugWarn()                                                 \
    ::Debug::DummyLogger()


#    define debugEmerg()                                                \
    ::Debug::DummyLogger()


#    define debugInfo()                                                 \
    ::Debug::DummyLogger()


#endif


namespace Debug
{
    void assertPanic();
    void assertPanic(const char *file, int line, const char *message);

    const char *stripModifiers(const char *name);
};
