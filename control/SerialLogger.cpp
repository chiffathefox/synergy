
#include <cstring>

#include "SerialLogger.hpp"


Stream *Debug::SerialLogger::sSerial = &Serial;


Debug::SerialLogger::SerialLogger(const char *level, const char *tag,
        const char *suffix, bool leftRoundBraceAsTagNull)
    : Logger(),
    mSuffix(suffix)
{
    sSerial->write(level);
    sSerial->write(":");

    if (leftRoundBraceAsTagNull) {
        const char *end = strchr(tag, '(');

        if (end == nullptr) {
            sSerial->write(tag);
        } else {
            sSerial->write(tag, end - tag);
        }
    } else {
        sSerial->write(tag);
    }

    sSerial->write(":");
}


Debug::SerialLogger::~SerialLogger()
{
    sSerial->write(mSuffix);
}


Debug::Logger &Debug::SerialLogger::operator<<(char value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(unsigned char value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(int value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(unsigned int value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(long value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(unsigned long value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(double value)
{
    sSerial->write(" ");
    sSerial->print(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(bool value)
{
    sSerial->write(" ");
    sSerial->write(value ? "true" : "false");

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(const char *value)
{
    sSerial->write(" ");
    sSerial->write(value);

    return *this;
}


Debug::Logger &Debug::SerialLogger::operator<<(const std::string &value)
{
    return *this << value.c_str();
}
