#include "SpBusServer.hpp"

#include "SpBusDefs.hpp"
#include "SpBusCodec.hpp"

#include "sockets/LinkRl.hpp"
#include "sockets/LinkAcceptor.hpp"

#include "utils/WrapBuffer.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"

#include <utility>
#include <limits>

namespace sg
{

SpBusServer::SpBusServer(Init const& init)
    : fsm(*this)
    , acceptor(init.acceptor)
    , link(std::unique_ptr<LinkRl>(new LinkRl(-1)))
    , rx(*link)
{
}

SpBusServer::~SpBusServer()
{
}

void SpBusServer::tickInd()
{
    fsm.tickInd();
}

char const* SpBusServer::name()
{
    return "SpBusServer";
}

int SpBusServer::accept()
{
    int fd = acceptor.accept();
    if (fd < 0)
    {
        return 0;
    }
    link->setHandl(fd);
    return 1;
}

int SpBusServer::process()
{
    int len = rx.receive(&rawBuffer[0], rawBuffer.size());
    
    if (!len)
    {
        return 0;
    }
    else if (len < 0)
    {
        return 1;
    }

    WrapBuffer rxBuf(&rawBuffer[0], len);

    if (Utils::crcode(rxBuf.cbegin() + 2, len - 2))
    {
        LM(LE, "CRCNOK");
        return 1;
    }

    SpBusFrame frame{};
    {
        SpBusCodec codec(rxBuf, frame);
        if (!codec.decode())
        {
            return 1;
        }
    }

    if (frame.hdr.fc != RDP)
    {
        LM(LE, "Unexpected function=%02X", frame.hdr.fc);
        return 1;
    }

    std::swap(frame.hdr.dad, frame.hdr.sad);
    frame.hdr.fc = RSP;
    frame.data.numInfos = frame.data.numPointers;

    if (frame.data.numInfos == 1)
    {
        float valueDouble = (rand() % std::numeric_limits<uint16_t>::max()) / 3.0;
        if (rand() % 2 == 0)
        {
            sprintf(frame.data.infos[0].value.param, "%.5f", valueDouble);
        }
        else
        {
            sprintf(frame.data.infos[0].value.param, "????");
        }
    }

    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
    {
        SpBusCodec codec(txBuf, frame);
        if (!codec.encode())
        {
            return 1;
        }
    }

    return link->write(txBuf.cbegin(), txBuf.size()) < 0;
}

void SpBusServer::reset()
{
    link->close();
}

}