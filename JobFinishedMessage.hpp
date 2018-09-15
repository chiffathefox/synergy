
#pragma once


#include "JobMessage.hpp"


namespace Synergy {
    class JobFinishedMessage;
};


class Synergy::JobFinishedMessage : public JobMessage
{


public:

    explicit JobFinishedMessage(char *buffer, int length);

};
