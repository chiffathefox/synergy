
#include "JobMessage.hpp"


Synergy::JobMessage::JobMessage(char *buffer, int length, Type type)
    : Message(buffer, length, type)
{
    mMessage = reinterpret_cast<JobMessageStruct *>(this->buffer());

    if (length == -1) {
        setBufferLength(sizeof (JobMessageStruct));

        return;
    }

    if (bufferLength() < sizeof (uint64_t)) {
        setOk(false);

        return;
    }
}


uint64_t Synergy::JobMessage::jobId() const
{
    return mMessage->jobId;
}


void Synergy::JobMessage::setJobId(uint64_t jobId)
{
    mMessage->jobId = jobId;
}
