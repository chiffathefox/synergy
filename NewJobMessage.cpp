
#include <cstddef>
#include <algorithm>

#include "NewJobMessage.hpp"


Synergy::NewJobMessage::NewJobMessage(char *buffer, int length)
    : JobMessage(buffer, length, Message::Type::NewJob)
{
    mMessage = reinterpret_cast<MessageStruct *>(
            this->buffer() + JobMessage::offset());

    if (length == -1) {
        return;
    }

    uint8_t n = bufferLength() - JobMessage::offset();

    setOk(n >= 1 && n - 1 == taskLength());
}


uint8_t Synergy::NewJobMessage::offset() const
{
    return bufferTotalSize();
}


void Synergy::NewJobMessage::setTask(const char *task, uint8_t taskLength)
{
    const uint8_t maxLength = sizeof (MessageStruct) -
        offsetof(MessageStruct, task);

    if (taskLength > maxLength) {
        taskLength = maxLength;
    }

    std::copy(task, task + taskLength, mMessage->task);
    mMessage->taskLength = taskLength;

    setBufferLength(JobMessage::offset() + sizeof (uint8_t) + taskLength);
}
