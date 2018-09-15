
#pragma once


#include "JobMessage.hpp"


namespace Synergy {
    class NewJobMessage;
};


class Synergy::NewJobMessage : public JobMessage
{


public:

    explicit NewJobMessage(char *buffer, int length);

};
