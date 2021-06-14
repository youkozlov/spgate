#include "RsBusServer.hpp"

#include "RsBusDefs.hpp"
#include "RsBusCodec.hpp"

#include "sockets/LinkRl.hpp"
#include "sockets/LinkAcceptorRl.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"

#include <utility>
#include <ctime>

namespace sg
{

namespace rsbus
{

RsBusServerFsm::RsBusServerFsm(RsBusServer& b)
    : bus(b)
    , state(State::init)
{
}

int RsBusServerFsm::process()
{
    switch (state)
    {
    case State::init:
        return init();
    case State::recvStartSequence:
        return recvStartSequence();
    case State::recvSessionReq:
        return recvSessionReq();
    case State::recvDataReq:
        return recvDataReq();
    }
    return 1;
}

int RsBusServerFsm::init()
{
    startSequenceCounter = 0;
    changeState(State::recvStartSequence);
    return 0;
}

int RsBusServerFsm::recvStartSequence()
{
    int result = bus.recvStartSequence();
    if (result < 0)
    {
        changeState(State::init);
    }
    else if (result)
    {
        startSequenceCounter += 1;
        if (startSequenceCounter == startSequenceLength)
        {
            changeState(State::recvSessionReq);
        }
        return 0;
    }
    return result;
}

int RsBusServerFsm::recvSessionReq()
{
    int result = bus.recvSessionReq();
    if (result < 0)
    {
        changeState(State::init);
    }
    else if (result)
    {
        changeState(State::recvDataReq);
        return 0;
    }
    return result;
}

int RsBusServerFsm::recvDataReq()
{
    int result = bus.recvDataReqLong();
    if (result < 0)
    {
        changeState(State::init);
    }
    else if (result)
    {
        return 0;
    }
    return result;
}

void RsBusServerFsm::changeState(State newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* RsBusServerFsm::toString(State st) const
{
    switch (st)
    {
    case State::init:
        return "Init";
    case State::recvStartSequence:
        return "RecvStartSequence";
    case State::recvSessionReq:
        return "RecvSessionReq";
    case State::recvDataReq:
        return "RecvDataReq";
    default:
        return "Invalid";
    }
}




RsBusServer::RsBusServer(Init const& init)
    : fsm(*this)
    , buffer(init.buffer)
    , acceptor(std::unique_ptr<LinkAcceptor>(new LinkAcceptorRl({init.ipAddr})))
    , link(std::unique_ptr<LinkRl>(new LinkRl(-1)))
    , rx(*link)
    , busFsm(*this)
{
}

RsBusServer::~RsBusServer()
{
}

void RsBusServer::tickInd()
{
    fsm.tickInd();
}

char const* RsBusServer::name()
{
    return "RsBusServer";
}

int RsBusServer::accept()
{
    int fd = acceptor->accept();
    if (fd < 0)
    {
        return 0;
    }
    link->setHandl(fd);
    return 1;
}

int RsBusServer::process()
{
    return busFsm.process();
}

int RsBusServer::recvStartSequence()
{
    int len = link->read(&rawBuffer[0], 1, 300);

    if (!len)
    {
        return len;
    }
    else if (len < 0)
    {
        return len;
    }

    if (rawBuffer[0] != 0xff)
    {
        LM(LE, "Unexpected symbol within start sequence: %02X", rawBuffer[0]);
        return -1;
    }

    return len;
}

int RsBusServer::recvSessionReq()
{
    int len = rx.receive(&rawBuffer[0], rawBuffer.size());
    
    if (!len)
    {
        return len;
    }
    else if (len == RsBusRx::invalid)
    {
        return 0;
    }
    else if (len < 0)
    {
        return len;
    }
    
    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decode())
    {
        return 0;
    }

    if (frame.qty != 4)
    {
        LM(LE, "Invalid session request length: %u", frame.qty);
        return 0;
    }

    if (frame.rc != SRC
     || frame.data[0] != 0x00
     || frame.data[1] != 0x00
     || frame.data[2] != 0x00
     || frame.data[3] != 0x00)
    {
        LM(LE, "Invalid content of session request");
        return -1;
    }

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    {
        frame.rc      = SRC;
        frame.data[0] = 0x47;
        frame.data[1] = 0x2A;
        frame.data[2] = 0x00;
        frame.qty     = 3;
        RsBusCodec codec(txBuf, frame);
        if (!codec.encode())
        {
            return 0;
        }
    }

    return link->write(txBuf.cbegin(), txBuf.size());
}

int RsBusServer::recvDataReqShort()
{
    int len = rx.receive(&rawBuffer[0], rawBuffer.size());
    
    if (!len)
    {
        return len;
    }
    else if (len == RsBusRx::invalid)
    {
        return 0;
    }
    else if (len < 0)
    {
        return len;
    }

    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decode())
    {
        return 0;
    }

    if (frame.rc != RDP)
    {
        LM(LE, "Unsupported function=%02X", frame.rc);
        return -1;
    }

    uint8_t a1  = frame.data[0];
    uint8_t a0  = frame.data[1];
    uint8_t qty = frame.data[2];

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    {
        if (a1 == 0x64)
        {
            time_t rawtime;
            time (&rawtime);
            struct tm * timeinfo = localtime(&rawtime);

            frame.data[0] = timeinfo->tm_year - 100;
            frame.data[1] = timeinfo->tm_mon + 1;
            frame.data[2] = timeinfo->tm_mday;
            frame.data[3] = timeinfo->tm_hour + 1;
            frame.data[4] = timeinfo->tm_min;
            frame.data[5] = timeinfo->tm_sec;
        }
        else
        {
            unsigned int const bufId = (a0 * 256) + a1;
            float const rsbusFloat   = Utils::encodeRsBus(buffer[bufId]);
            memcpy(frame.data, &rsbusFloat, sizeof(float));
        }
        frame.qty     = qty;
        RsBusCodec codec(txBuf, frame);
        if (!codec.encode())
        {
            return 0;
        }
    }

    return link->write(txBuf.cbegin(), txBuf.size());
}

int RsBusServer::recvDataReqLong()
{
    int len = rx.receiveLong(&rawBuffer[0], rawBuffer.size());
    
    if (!len)
    {
        return len;
    }
    else if (len == RsBusRx::invalid)
    {
        return 0;
    }
    else if (len < 0)
    {
        return len;
    }

    WrapBuffer rxBuf(&rawBuffer[0], len);

    RsBusFrame frame;

    RsBusCodec codec(rxBuf, frame);

    if (!codec.decodeReqLong())
    {
        return -1;
    }
    
    if (frame.rc != RM4)
    {
        LM(LE, "Unsupported function=%02X", frame.rc);
        return -1;
    }

    if (frame.tag != TAG_PNUM)
    {
        LM(LE, "Unsupported tag=%02X", frame.tag);
        return -1;
    }

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    {
        float const rsbusFloat = buffer[frame.prm];
        memcpy(frame.data, &rsbusFloat, sizeof(float));
        frame.tag = TAG_IEEFloat;
        RsBusCodec codec(txBuf, frame);
        if (!codec.encodeRspLong())
        {
            return 0;
        }
    }
    return link->write(txBuf.cbegin(), txBuf.size());
}

void RsBusServer::reset()
{
    link->close();
}

}
}