#include "RsBusCodec.hpp"
#include "utils/Logger.hpp"
#include "utils/WrapBuffer.hpp"


namespace sg
{

namespace
{

uint8_t calcCs(RsBusFrame const& frame, uint8_t dataLen)
{
    uint16_t sum = frame.nt + frame.rc;
    while (dataLen--)
    {
        LM(LE, "dataLen=%u", dataLen);
        sum += frame.data[dataLen];
    }
    return ~sum;
}

bool validateNt(uint8_t nt)
{
    if (nt != 255 && nt > 99)
    {
        LM(LE, "Invalid param, nt=%u", nt);
        return false;
    }
    return true;
}

}

/*
1   Код начала кадра (10Н)
2   Групповой номер прибора (NT)
3   Код запроса
4   Поле 1
5   Поле 2
6   Поле 3
7   Поле 4
8   Контрольная сумма (КС)
9   Код конца кадра (16Н)
*/

RsBusCodec::RsBusCodec(WrapBuffer& b, RsBusFrame& f)
    : buf(b)
    , frame(f)
{
}

bool RsBusCodec::encodeStartSequence()
{
    for (int i = 0; i < 16; i += 1)
    {
        buf.write(0xFF);
    }
    return true;
}

//
// 10Н
// NT
// 3FH
// 00H
// 00H
// 00H
// 00H
// КС
// 16Н
//
bool RsBusCodec::encodeSessionReq()
{
    if (!validateNt(frame.nt))
    {
        return false;
    }

    buf.write(FSC);
    buf.write(frame.nt);
    buf.write(0x3F);
    buf.write(0x00);
    buf.write(0x00);
    buf.write(0x00);
    buf.write(0x00);
    buf.write(calcCs(frame, dataRequestLength));
    buf.write(FEC);

    return true;
}

//
// 10Н
// NT
// 52Н
// А1
// А0
// Кб
// 00H
// КС
// 16Н
//
bool RsBusCodec::encodeDataReq()
{
    if (!validateNt(frame.nt))
    {
        return false;
    }

    if (frame.rc != RDRAM)
    {
        LM(LE, "Unsupported function=%02X", frame.rc);
        return false;
    }

    buf.write(FSC);
    buf.write(frame.nt);
    buf.write(frame.rc);
    buf.write(frame.a1);
    buf.write(frame.a0);
    buf.write(frame.qty);
    buf.write(0x00);
    buf.write(calcCs(frame, dataRequestLength));
    buf.write(FEC);

    return true;
}

//
// 10Н
// NT
// 3FH
// 54H
// 29Н
// 00Н
// КС
// 16Н
//
bool RsBusCodec::decodeSessionRsp()
{
    if (buf.capacity() != 8)
    {
        LM(LE, "Invalid length of session respond: %u", buf.capacity());
        return false;
    }

    unsigned char ch;

    buf.read(ch);

    if (ch != FSC)
    {
        LM(LE, "Unexpected start symbol: %02X", ch);
        return false;
    }

    buf.read(frame.nt);

    buf.read(frame.rc);

    buf.read(frame.data[0]);

    buf.read(frame.data[1]);

    buf.read(frame.data[2]);

    return true;
}

// 10Н
// NT
// 52Н
// ДампОЗУ 0 байт
// ДампОЗУ 1 байт
// ДампОЗУ 2 байт
// ДампОЗУ 3 байт
// КС
// 16Н

bool RsBusCodec::decodeDataRsp()
{
    if (buf.capacity() != 9)
    {
        LM(LE, "Unexpected length of data respond: %u", buf.capacity());
        return false;
    }

    unsigned char ch;

    buf.read(ch);

    if (ch != FSC)
    {
        LM(LE, "Unexpected start symbol: %02X", ch);
        return false;
    }

    buf.read(frame.nt);

    buf.read(frame.rc);

    if (frame.rc != RDRAM)
    {
        LM(LE, "Unsupported function=%02X", frame.rc);
        return false;
    }

    buf.read(frame.data[0]);

    buf.read(frame.data[1]);

    buf.read(frame.data[2]);

    buf.read(frame.data[3]);

    return true;
}

}