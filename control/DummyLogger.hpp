
#pragma once


#include "Logger.hpp"


namespace Debug
{
    class DummyLogger;
};


class Debug::DummyLogger : public Logger
{


public:

    inline virtual Logger &operator<<(char) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(unsigned char) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(int) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(unsigned int) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(long) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(unsigned long) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(double) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(bool) override
    {
        return *this;
    }


    inline virtual Logger &operator<<(const char *) override
    {
        return *this;
    }


};
