
#pragma once


#include "JobMessage.hpp"


namespace Synergy {
    class JobFinishedMessage;
};


class Synergy::JobFinishedMessage : public JobMessage
{


public:

    JobFinishedMessage(char *buffer = nullptr, int length = -1);

};
