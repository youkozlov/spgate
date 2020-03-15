#include "SpBusCodec.hpp"
#include "SpBusDefs.hpp"
#include "utils/Utils.hpp"
#include "utils/WrapBuffer.hpp"
#include "utils/Logger.hpp"
#include <cstdlib>

namespace sg
{

SpBusCodec::SpBusCodec(WrapBuffer& b, SpBusFrame& f)
    : state(State::idle)
    , subs(SubState::procPointer)
    , buf(b)
    , frame(f)
{
}

bool SpBusCodec::decode()
{
    state = State::idle;
    subs  = SubState::procPointer;

    unsigned char ch;

    while (buf.read(ch))
    {
        if (ch == DLE)
        {
            changeState(State::procCmd);
            continue;
        }

        switch (state)
        {
        case State::procCmd:
            procCmd(ch);
        break;
        case State::procDad:
            procDad(ch);
        break;
        case State::procSad:
            procSad(ch);
        break;
        case State::procFunc:
            procFunc(ch);
        break;
        case State::procDataHead:
            procDataHead(ch);
        break;
        case State::procDataSet:
            procDataSet(ch);
        break;
        case State::done:
            return true;
        default:
            LM(LE, "Decode is incompleted");
            return false;
        }
    }

    LM(LE, "Unexpected end of frame");
    return false;
}

void SpBusCodec::procCmd(unsigned char cmd)
{
    switch (cmd)
    {
    case SOH:
        changeState(State::procDad);
    break;
    case ISI:
        changeState(State::procFunc);
    break;
    case STX:
        changeState(State::procDataSet);
    break;
    case ETX:
        changeState(State::done);
    break;
    default:
    {
        LM(LE, "Unexpected command");
        changeState(State::error);
    }
    break;
    }
}

void SpBusCodec::procDad(unsigned char ch)
{
    LM(LD, "dad=%u", ch);

    frame.hdr.dad = ch;
    changeState(State::procSad);
}

void SpBusCodec::procSad(unsigned char ch)
{
    LM(LD, "sad=%u", ch);

    frame.hdr.sad = ch;
    changeState(State::idle);
}

void SpBusCodec::procFunc(unsigned char ch)
{
    LM(LD, "fc=%02X", ch);

    frame.hdr.fc = ch;
    frame.hdr.size = 0;
    switch (frame.hdr.fc)
    {
    case RDP:
    case RSP:
        changeState(State::procDataHead);
    break;
    default:
    {
        LM(LE, "Unsupported function=%02X", frame.hdr.fc);
        changeState(State::error);
    }
    }
}

void SpBusCodec::procDataHead(unsigned char ch)
{
    if ((frame.hdr.size + 1u) >= Utils::size(frame.hdr.data))
    {
        LM(LE, "Unexpected size of DataHead");
        changeState(State::error);
        return;
    }
    frame.hdr.data[frame.hdr.size++] = ch;
}

void SpBusCodec::procDataSet(unsigned char ch)
{
    switch (subs)
    {
    case SubState::procPointer:
        procPointer(ch);
    break;
    case SubState::procChan:
        procChan(ch);
    break;
    case SubState::procPrm:
        procPrm(ch);
    break;
    case SubState::procInfo:
        procInfo(ch);
    break;
    case SubState::procValue:
        procValue(ch);
    break;
    case SubState::procUnit:
        procUnit(ch);
    break;
    case SubState::procTs:
        procTs(ch);
    break;
    default:
    {
        LM(LE, "Unexpected subState");
        changeState(State::error);
    }
    break;
    }
}

void SpBusCodec::procPointer(unsigned char ch)
{
    if (ch == HT)
    {
        changeSubState(SubState::procChan);
    }
    else
    {
        LM(LE, "Unexpected symbol=%02X", ch);
        changeState(State::error);
    }
}

void SpBusCodec::procChan(unsigned char ch)
{
    auto& ptr = frame.data.pointers[frame.data.numPointers];
    SpBusDataSetItem& it = ptr.chan;

    if (ch == HT)
    {
        it.param[it.size] = 0;
        changeSubState(SubState::procPrm);
        return;
    }

    if ((it.size + 1u) >= Utils::size(it.param))
    {
        LM(LE, "Unexpected size of chan");
        changeState(State::error);
        return;
    }
    it.param[it.size++] = ch;
}

void SpBusCodec::procPrm(unsigned char ch)
{
    auto& ptr = frame.data.pointers[frame.data.numPointers];
    SpBusDataSetItem& it = ptr.prm;

    if (ch == FF)
    {
        if (frame.data.numPointers >= Utils::size(frame.data.pointers))
        {
            LM(LE, "Unexpected number of pointers");
            changeState(State::error);
            return;
        }
        it.param[it.size] = 0;

        frame.data.numPointers += 1;
        switch (frame.hdr.fc)
        {
        case RDP:
            changeSubState(SubState::procPointer);
        break;
        case RSP:
            changeSubState(SubState::procInfo);
        break;
        }
        return;
    }

    if ((it.size + 1u) >= Utils::size(it.param))
    {
        LM(LE, "Unexpected size of param");
        changeState(State::error);
        return;
    }
    it.param[it.size++] = ch;
}

void SpBusCodec::procInfo(unsigned char ch)
{
    if (ch == HT)
    {
        changeSubState(SubState::procValue);
    }
    else
    {
        LM(LE, "Unexpected symbol=%02X", ch);
        changeState(State::error);
    }
}

void SpBusCodec::procValue(unsigned char ch)
{
    auto& inf = frame.data.infos[frame.data.numInfos];
    SpBusDataSetItem& it = inf.value;

    if (ch == HT || ch == FF)
    {
        if (frame.data.numInfos >= Utils::size(frame.data.infos))
        {
            LM(LE, "Unexpected number of infos");
            changeState(State::error);
            return;
        }
        frame.data.numInfos += 1;

        it.param[it.size] = 0;
        changeSubState(ch == HT ? SubState::procUnit : SubState::procPointer);
        return;
    }

    if ((it.size + 1u) >= Utils::size(it.param))
    {
        LM(LE, "Unexpected size of value");
        changeState(State::error);
        return;
    }
    it.param[it.size++] = ch;
}

void SpBusCodec::procUnit(unsigned char ch)
{
    auto& inf = frame.data.infos[frame.data.numInfos];
    SpBusDataSetItem& it = inf.unit;

    if (ch == HT || ch == FF)
    {
        if (frame.data.numInfos >= Utils::size(frame.data.infos))
        {
            LM(LE, "Unexpected number of infos");
            changeState(State::error);
            return;
        }
        frame.data.numInfos += 1;

        it.param[it.size] = 0;
        changeSubState(ch == HT ? SubState::procTs : SubState::procPointer);
        return;
    }

    if ((it.size + 1u) >= Utils::size(it.param))
    {
        LM(LE, "Unexpected size of unit");
        changeState(State::error);
        return;
    }
    it.param[it.size++] = ch;
}

void SpBusCodec::procTs(unsigned char ch)
{
    auto& inf = frame.data.infos[frame.data.numInfos];
    SpBusDataSetItem& it = inf.ts;

    if (ch == HT || ch == FF)
    {
        if (frame.data.numInfos >= Utils::size(frame.data.infos))
        {
            LM(LE, "Unexpected number of infos");
            changeState(State::error);
            return;
        }
        frame.data.numInfos += 1;

        it.param[it.size] = 0;
        changeSubState(SubState::procPointer);
        return;
    }

    if ((it.size + 1u) >= Utils::size(it.param))
    {
        LM(LE, "Unexpected size of ts");
        changeState(State::error);
        return;
    }
    it.param[it.size++] = ch;
}

bool SpBusCodec::encode()
{
    if (frame.data.numPointers > Utils::size(frame.data.pointers) || !frame.data.numPointers)
    {
        LM(LE, "Unexpected number of pointers=%u", frame.data.numPointers);
        return false;
    }

    buf.write(DLE);
    buf.write(SOH);
    buf.write(frame.hdr.dad);
    buf.write(frame.hdr.sad);
    buf.write(DLE);
    buf.write(ISI);
    buf.write(frame.hdr.fc);
    buf.write(frame.hdr.data);
    buf.write(DLE);
    buf.write(STX);

    bool result;
    switch (frame.hdr.fc)
    {
    case RDP:
        result = encodeRdp();
    break;
    case RSP:
        result = encodeRsp();
    break;
    default:
    {
        LM(LE, "Unsupported function=%02X", frame.hdr.fc);
        return false;
    }
    }

    if (!result)
    {
        LM(LE, "Encode incompleted");
        return false;
    }

    buf.write(DLE);
    buf.write(ETX);
    unsigned int crc = Utils::crcode(buf.cbegin() + 2, buf.size() - 2);
    buf.write(crc >> 8);
    buf.write(crc);

    return true;
}

bool SpBusCodec::encodeRdp()
{
    for (unsigned int i = 0; i < frame.data.numPointers; ++i)
    {
        auto& ptr = frame.data.pointers[i];
        buf.write(HT);
        buf.write(ptr.chan.param);
        buf.write(HT);
        buf.write(ptr.prm.param);
        buf.write(FF);
    }
    return true;
}

bool SpBusCodec::encodeRsp()
{
    if (frame.data.numPointers != frame.data.numInfos)
    {
        LM(LE, "Number of pointers and infos isn't equal");
        return false;
    }

    for (unsigned int i = 0; i < frame.data.numPointers; ++i)
    {
        auto& ptr = frame.data.pointers[i];
        buf.write(HT);
        buf.write(ptr.chan.param);
        buf.write(HT);
        buf.write(ptr.prm.param);
        buf.write(FF);

        auto& inf = frame.data.infos[i];
        buf.write(HT);
        buf.write(inf.value.param);
        buf.write(FF);
    }
    return true;
}

void SpBusCodec::changeState(State newSt)
{
    LM(LD, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* SpBusCodec::toString(State st) const
{
    switch (st)
    {
    case State::procCmd:
        return "ProcCmd";
    case State::procDad:
        return "ProcDad";
    case State::procSad:
        return "ProcSad";
    case State::procFunc:
        return "ProcFunc";
    case State::procDataHead:
        return "ProcDataHead";
    case State::procDataSet:
        return "ProcDataSet";
    case State::idle:
        return "Idle";
    case State::done:
        return "Done";
    case State::error:
        return "Error";
    default:
        return "Invalid";
    }
}

void SpBusCodec::changeSubState(SubState newSt)
{
    LM(LD, "Change subState: %s -> %s", toString(subs), toString(newSt));
    subs = newSt;
}

char const* SpBusCodec::toString(SubState st) const
{
    switch (st)
    {
    case SubState::procPointer:
        return "ProcPointer";
    case SubState::procChan:
        return "ProcChan";
    case SubState::procPrm:
        return "ProcPrm";
    case SubState::procInfo:
        return "ProcInfo";
    case SubState::procValue:
        return "ProcValue";
    case SubState::procUnit:
        return "ProcUnit";
    case SubState::procTs:
        return "ProcTs";
    default:
        return "Invalid";
    }
}

}