
#pragma once


#include <cstdint>


namespace Synergy
{
    class Message;
};


class Synergy::Message
{

    bool mOk;
    char *mBuffer;


public:


    enum Type : uint8_t
    {
        Beacon,
        NewJob,
        JobFinished
    };


    explicit Message(char *buffer, int length, Type type);
    ~Message();

    char *raw() const;
    bool ok() const;
    Type type() const;
    uint8_t bufferLength() const;
    char *buffer() const;


protected:

    void setOk(bool ok);
    void setType(Type type);
    void setBufferLength(uint8_t bufferLength);


private:

    struct MessageStruct
    {
        Type type;
        uint8_t bufferLength;
        char buffer[255];
    };


    MessageStruct *mMessage;

};
