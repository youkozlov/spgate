#include "GateSpBus.hpp"
#include "sockets/LinkRl.hpp"
#include "types/ParamsDefs.hpp"
#include "utils/WrapBuffer.hpp"

namespace sg
{

namespace
{

const char DLE = 0x10;
const char SOH = 0x01;
const char ISI = 0x1F;
const char RDP = 0x1D;
const char STX = 0x02;
const char ETX = 0x03;
const char HT  = 0x09;
const char FF  = 0x0C;

int CRCode(unsigned char const* msg, int len)
{
    int crc = 0;
    while (len-- > 0)
    {
        crc = crc ^ (int)*msg++ << 8;
        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}
}

GateSpBus::GateSpBus(Init const& init)
    : gateParams(init.gateParams)
    , storage(init.parser)
    , regs(init.regs)
    , fsm(*this)
    , currentParamId(0)
{
    link = std::unique_ptr<Link>(new LinkRl(gateParams.gateAddr));
}

GateSpBus::~GateSpBus()
{
}

bool GateSpBus::configure()
{
    return storage.configure(gateParams);
}

void GateSpBus::tickInd()
{
    fsm.tickInd();
}

int GateSpBus::connect()
{
    return link->connect();
}

int GateSpBus::send()
{
    WrapBuffer buf(&rawBuffer[0], rawBuffer.size());

    auto& item   = storage.getItem((currentParamId++) % storage.getNumItems());
    auto& device = item.device;
    auto& prms   = item.prms;

//  HEAD
    buf.write(DLE);
    buf.write(SOH);
    buf.write(device.addr);
    buf.write(gateParams.addr);
    buf.write(DLE);
    buf.write(ISI);
    buf.write(prms.func);
    buf.write(DLE);
    buf.write(STX);
//  --

    buf.write(HT);
    buf.encode(prms.chan);
    buf.write(HT);
    buf.encode(prms.addr);
    buf.write(FF);

//  TILE
    buf.write(DLE);
    buf.write(ETX);
    int crc = CRCode(buf.cbegin() + 2, buf.size() - 2);
    buf.write(crc >> 8);
    buf.write(crc);
//  --
    return link->write(buf.cbegin(), buf.size());
}

int GateSpBus::receive()
{
    return 0;
}

unsigned int GateSpBus::period()
{
    return gateParams.readPeriod;
}

void GateSpBus::reset()
{
    currentParamId = 0;
    link->close();
}

}