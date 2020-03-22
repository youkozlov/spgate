#include "RsBusCodec.hpp"
#include "utils/Logger.hpp"
#include "utils/WrapBuffer.hpp"

namespace sg
{

namespace rsbus
{

uint8_t calcCs(RsBusFrame const& frame, uint8_t dataLen)
{
    uint16_t sum = frame.nt + frame.rc;
    while (dataLen--)
    {
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

// session request
//
// 10Н NT 3FH 00H 00H 00H 00H КС 16Н

// session respond 941
//
// 10Н NT 3FH 54H 29Н 00Н КС 16Н

// data request
//
// 10Н NT 52Н А1 А0 Кб 00H КС 16Н

// data respond
//
// 10Н NT 52Н [ДампОЗУ 4 байта] КС 16Н

bool RsBusCodec::encode()
{
    if (!validateNt(frame.nt))
    {
        return false;
    }

    buf.write(FSC);
    buf.write(frame.nt);
    buf.write(frame.rc);
    buf.write(frame.data, frame.qty);
    buf.write(calcCs(frame, frame.qty));
    buf.write(FEC);

    return true;
}

bool RsBusCodec::decode()
{
    unsigned char ch;
    if (!buf.read(ch))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (ch != FSC)
    {
        LM(LE, "Unexpected start symbol: %02X", ch);
        return false;
    }

    if (!buf.read(frame.nt))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (!validateNt(frame.nt))
    {
        return false;
    }

    if (!buf.read(frame.rc))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    frame.qty = 0;
    while ((buf.size() + 2) < buf.capacity())
    {
        buf.read(frame.data[frame.qty++]);
    }

    if (!buf.read(ch))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    uint8_t const expKc = calcCs(frame, frame.qty);
    if (ch != expKc)
    {
        LM(LE, "Given KC is incorrect: %02X, expected KC: %02X", ch, expKc);
        return false;
    }

    if (!buf.read(ch))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (ch != FEC)
    {
        LM(LE, "Unexpected end symbol: %02X", ch);
        return false;
    }

    return true;
}
}
}