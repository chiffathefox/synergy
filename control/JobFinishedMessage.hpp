
#pragma once


#include "JobMessage.hpp"


namespace Synergy {
    class JobFinishedMessage;
};


class Synergy::JobFinishedMessage : public JobMessage
{


public:

    JobFinishedMessage(char *buffer = nullptr, size_t length = -1);

};
