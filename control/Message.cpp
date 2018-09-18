
#include <algorithm>

#include "Message.hpp"


Synergy::Message::Message(char *buffer, size_t length, Type type)
    : mOk(false)
{
    if (buffer == nullptr) {
        setType(type);
        setBufferLength(0);

        return;
    }

    if (length < offsetof(MessageStruct, buffer) || length > maxRawLength()) {
        return;
    }

    std::copy(buffer, buffer + length, reinterpret_cast<char *>(&mMessage));
    setOk(this->type() == type || type == Type::None);
}


uint8_t Synergy::Message::offset() const
{
    return 0;
}
