
#include <stdio.h>

#include "StringUtils.hpp"


std::string StringUtils::toString(int value)
{
    char buf[sizeof ("-9223372036854775808")];

    sprintf(buf, "%d", value);

    return std::string(buf);
}


std::string StringUtils::toString(unsigned int value)
{
    char buf[sizeof ("18446744073709551616")];

    sprintf(buf, "%u", value);

    return std::string(buf);
}
