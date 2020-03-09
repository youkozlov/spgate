#include "GateSpBus.hpp"
#include "DataRequest.hpp"
#include "sockets/LinkRl.hpp"
#include "types/ParamsDefs.hpp"

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

int CRCode(char *msg, int len)
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
{
    link = std::unique_ptr<LinkRl>(new LinkRl(gateParams.gateAddr));
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
}

bool GateSpBus::request(DataRequest const& req)
{
    buf.reset();

//  HEAD
    buf.addByte(DLE);
    buf.addByte(SOH);
    buf.addByte(req.dad);
    buf.addByte(req.sad);
    buf.addByte(DLE);
    buf.addByte(ISI);
    buf.addByte(RDP);
    buf.addByte(DLE);
    buf.addByte(STX);
//  --

    buf.addByte(HT);
    buf.encode(req.chanId);
    buf.addByte(HT);
    buf.encode(req.paramId);
    buf.addByte(FF);

//  TILE
    buf.addByte(DLE);
    buf.addByte(ETX);
    int crc = CRCode(buf.get(2), buf.size() - 2);
    buf.addByte(crc >> 8);
    buf.addByte(crc);
//  --
    
    link->write(buf.get(), buf.size());

    return true;
}

DataRespond GateSpBus::respond()
{
    DataRespond rsp;
    return rsp;
}

}