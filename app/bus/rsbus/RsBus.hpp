#pragma once

namespace sg
{

namespace rsbus
{

class RsBus
{
public:
    virtual ~RsBus() {}

    virtual int connect() = 0;

    virtual int sendStartSequence() = 0;

    virtual int sendSessionReq() = 0;

    virtual int recvSessionRsp() = 0;

    virtual int sendDataReq() = 0;

    virtual int recvDataRsp() = 0;

    virtual unsigned int period() const = 0;

    virtual void reset() = 0;

    virtual void timeout() = 0;
};

}
}