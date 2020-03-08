#include "FeProcessorSpNet.hpp"
#include "DataRequest.hpp"
#include "SerialPortRl.hpp"

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

FeProcessorSpNet::FeProcessorSpNet(Init const& init)
    : port(init.port)
{
}

FeProcessorSpNet::~FeProcessorSpNet()
{
}
    
bool FeProcessorSpNet::request(DataRequest const& req)
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
    
    port.send(buf.get(), buf.size());

    return true;
}
    
DataRespond FeProcessorSpNet::respond()
{
    DataRespond rsp;
    return rsp;
}

}