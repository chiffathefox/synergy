
#pragma once


#include "JobMessage.hpp"


namespace Synergy
{
    class JobFinishedAcceptedMessage;
};


class Synergy::JobFinishedAcceptedMessage : public JobMessage
{


public:

    JobFinishedAcceptedMessage(char *buffer = nullptr, size_t length = -1);

};
