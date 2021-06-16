#include "RsBusCodec.hpp"
#include "utils/Logger.hpp"
#include "utils/WrapBuffer.hpp"
#include "utils/Utils.hpp"

namespace sg
{

namespace rsbus
{

uint8_t calcCs(RsBusFrame const& frame, uint8_t dataLen)
{
    uint8_t sum = frame.nt + frame.rc;
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

bool RsBusCodec::encodeReqLong()
{
    if (!validateNt(frame.nt))
    {
        return false;
    }

    buf.write(FSC);
    buf.write(frame.nt);
    buf.write(FRM);
    buf.write(frame.id);
    buf.write(frame.atr);
    buf.write(0x06);
    buf.write(0x00);
    buf.write(frame.rc);
    buf.write(frame.tag);
    buf.write(0x03);
    buf.write(frame.ch);
    buf.write(static_cast<uint8_t>(frame.prm));
    buf.write(static_cast<uint8_t>(frame.prm >> 8));
    unsigned int crc = Utils::crcode(buf.cbegin() + 1, buf.size() - 1);
    buf.write(crc >> 8);
    buf.write(crc);

    return true;
}

bool RsBusCodec::encodeRspLong()
{
    if (!validateNt(frame.nt))
    {
        return false;
    }

    buf.write(FSC);
    buf.write(frame.nt);
    buf.write(FRM);
    buf.write(frame.id);
    buf.write(frame.atr);
    buf.write(0x07);
    buf.write(0x00);
    buf.write(frame.rc);
    buf.write(frame.tag);
    buf.write(0x04);
    buf.write(frame.data[0]);
    buf.write(frame.data[1]);
    buf.write(frame.data[2]);
    buf.write(frame.data[3]);
    unsigned int crc = Utils::crcode(buf.cbegin() + 1, buf.size() - 1);
    buf.write(crc >> 8);
    buf.write(crc);

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

bool RsBusCodec::decodeRspLong()
{
    if (Utils::crcode(buf.cbegin() + 1, buf.size() - 1))
    {
        LM(LE, "Invalid crc");
        return false;
    }

    unsigned char dummy;

    // FSC
    if (!buf.read(dummy))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // NT
    if (!buf.read(frame.nt))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (!validateNt(frame.nt))
    {
        return false;
    }

    // FRM
    if (!buf.read(dummy))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // ID
    if (!buf.read(frame.id))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // ATR
    if (!buf.read(frame.atr))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    uint8_t d0, d1;
    if (!buf.read(d0) || !buf.read(d1))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (d0 != 0x7)
    {
        LM(LE, "Unexpected tb size(%u), expected 0x7", d0);
        return false;
    }

    // FC
    if (!buf.read(frame.rc))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // TAG
    if (!buf.read(frame.tag))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    uint8_t d2;
    if (!buf.read(d2))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (d2 != 0x4)
    {
        LM(LE, "Unexpected param size(%u), expected 0x4", d2);
        return false;
    }

    frame.qty = 4;
    if (!buf.read(frame.data[0]) || !buf.read(frame.data[1])
        || !buf.read(frame.data[2]) || !buf.read(frame.data[3]))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    return true;
}

bool RsBusCodec::decodeReqLong()
{
    if (Utils::crcode(buf.cbegin() + 1, buf.size() - 1))
    {
        LM(LE, "Invalid crc");
        return false;
    }

    unsigned char dummy;

    // FSC
    if (!buf.read(dummy))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // NT
    if (!buf.read(frame.nt))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    if (!validateNt(frame.nt))
    {
        return false;
    }

    // FRM
    if (!buf.read(dummy))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // ID
    if (!buf.read(frame.id))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // ATR
    if (!buf.read(frame.atr))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }
    
    uint8_t d0, d1;
    if (!buf.read(d0) || !buf.read(d1))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // FC
    if (!buf.read(frame.rc))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // TAG
    if (!buf.read(frame.tag))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    uint8_t d2;
    if (!buf.read(d2) || d2 != 0x3)
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    // CH
    if (!buf.read(frame.ch))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }

    uint8_t a0, a1;
    if (!buf.read(a0) || !buf.read(a1))
    {
        LM(LE, "Unexpected tb end");
        return false;
    }
    frame.prm = (a1 << 8) | a0;

    return true;
}
}
}