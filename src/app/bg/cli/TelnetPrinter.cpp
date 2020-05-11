#include "TelnetPrinter.hpp"
#include "TelnetServer.hpp"
#include "sockets/Link.hpp"
#include "TelnetDefs.hpp"
#include "utils/WrapBuffer.hpp"
#include "utils/Logger.hpp"

namespace sg
{
namespace cli
{

char const* invalidCommand = "\r\n?Invalid command\r\n";

TelnetPrinter::TelnetPrinter(Link& link_, TelnetServer& srv_)
    : state(State::readLine)
    , link(link_)
    , srv(srv_)
    , lineLen(0)
    , optGaEnabled(false)
    , optEchoEnabled(false)
    , optLmEnabled(false)
    , lastCmdLen(0)
{
}

TelnetPrinter::~TelnetPrinter()
{
}

int TelnetPrinter::process()
{
    return receive();
}

int TelnetPrinter::init()
{
    WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());

    txBuf.write(IAC);
    txBuf.write(WILL);
    txBuf.write(OPT_GA);

    txBuf.write(IAC);
    txBuf.write(WILL);
    txBuf.write(OPT_ECHO);

    txBuf.write(IAC);
    txBuf.write(DO);
    txBuf.write(OPT_NAWS);

    txBuf.write(0x1b);
    txBuf.write(0x5b);
    txBuf.write(0x48);

    txBuf.write(0x1b);
    txBuf.write(0x5b);
    txBuf.write(0x32);
    txBuf.write(0x4a);

    if (link.write(txBuf.cbegin(), txBuf.size()) < 0)
    {
        LM(LE, "Can not send message");
        return 1;
    }

    char const* hello = "SpGate command line interface\r\n\r\ncli> ";
    if (link.write(hello, strlen(hello)) < 0)
    {
        LM(LE, "Can not send message");
        return 1;
    }

    changeState(State::readLine);

    return 0;
}

int TelnetPrinter::receive()
{
    unsigned int rxLen = 0;
    while (1)
    {
        int len = link.read(&rawBuffer[rxLen], 1, 50);
        if (len < 0)
        {
            return len;
        }
        else if (!len)
        {
            if (!rxLen)
            {
                return 0;
            }
            break;
        }
        if (rxLen >= rawBuffer.size())
        {
            LM(LE, "Received message is too huge");
            return 0;
        }
        rxLen += len;
    }

    WrapBuffer rxBuf(&rawBuffer[0], rxLen);
  
    unsigned char ch;

    while (rxBuf.read(ch))
    {
        LM(LD, "RX: %02X", ch);

        if (ch == IAC)
        {
            changeState(State::readCmd);
            continue;
        }
        if (ch == 0x1b)
        {
            changeState(State::readEsc);
            continue;
        }

        switch (state)
        {
        case State::readCmd:
            readCmd(ch);
        break;
        case State::readCmdOption:
            readCmdOption(ch);
        break;
        case State::readCmdSubOption:
            readCmdSubOption(ch);
        break;
        case State::readEsc:
            readEsc(ch);
        break;
        case State::readEscOption:
            readEscOption(ch);
        break;
        case State::readLine:
            readLine(ch);
        break;
        case State::error:
            return 1;
        default:
            LM(LE, "Unexpected state");
            return 1;
        }
    }

    return 0;
}

void TelnetPrinter::readLine(unsigned char ch)
{
    if (lineLen >= line.size())
    {
        LM(LE, "Unexpected length of line");
        changeState(State::error);
        return;
    }
    if (ch == 0x0D || ch == 0x0A)
    {
        return;
    }
    else if (ch == 0)
    {
        if (lineLen)
        {
            std::memset(&lastCmd[0], 0, lastCmd.size());
            std::memcpy(&lastCmd[0], &line[0], lineLen);
            lastCmdLen = lineLen;

            WrapBuffer txBuf(&rawBuffer[0], rawBuffer.size());
        
            line[lineLen] = 0;
            int result = srv.processLine(&line[0], lineLen, txBuf);
            lineLen = 0;
            if (result)
            {
                link.write(invalidCommand, std::strlen(invalidCommand));
            }
            else
            {
                link.write(txBuf.cbegin(), txBuf.size());
            }
        }
        link.write("\r\ncli> ", 7);
        return;
    }
    else if (ch == 0x03)
    {
        lineLen = 0;
        link.write("\r\ncli> ", 7);
        return;
    }
    else if (ch < 32 || ch > 126)
    {
        return;
    }

    if (optEchoEnabled)
    {
        link.write(&ch, 1);
    }

    line[lineLen++] = ch;
}

void TelnetPrinter::readCmd(unsigned char ch)
{
    switch (ch)
    {
    case DONT:
    case DO:
    case WONT:
    case WILL:
        cmd = ch;
        changeState(State::readCmdOption);
    break;
    case SB:
        changeState(State::readCmdSubOption);
    break;
    case SE:
        changeState(State::readLine);
    break;
    default:
        LM(LE, "Unexpected symbol = %u", ch);
        changeState(State::error);
    break;
    }
}

void TelnetPrinter::readCmdOption(unsigned char ch)
{
    switch (ch)
    {
    case OPT_ECHO:
        optEchoEnabled = (cmd == DO);
        changeState(State::readLine);
    break;
    case OPT_GA:
        optGaEnabled = (cmd == DO);
        changeState(State::readLine);
    break;
    case OPT_LM:
        optLmEnabled = (cmd == DO);
        changeState(State::readLine);
    break;
    case OPT_NAWS:
        changeState(State::readLine);
    break;
    default:
        LM(LE, "Unsupported option = %u", ch);
        changeState(State::error);
    break;
    }
}

void TelnetPrinter::readCmdSubOption(unsigned char)
{
}

void TelnetPrinter::readEsc(unsigned char ch)
{
    if (ch != 0x5b)
    {
        LM(LE, "Unsupported esc sequence");
        changeState(State::error);
        return;
    }
    changeState(State::readEscOption);
}

void TelnetPrinter::readEscOption(unsigned char ch)
{
    if (ch == 0x41 && lastCmdLen && (std::memcmp(&line[0], &lastCmd[0], lastCmdLen) || lastCmdLen != lineLen))
    {
        link.write(&lastCmd[0], lastCmdLen);
        std::memcpy(&line[0], &lastCmd[0], lastCmdLen);
        lineLen = lastCmdLen;
    }
    changeState(State::readLine);
}

void TelnetPrinter::changeState(State newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

char const* TelnetPrinter::toString(State st) const
{
    switch (st)
    {
    case State::readCmd:
        return "ReadCmd";
    case State::readCmdOption:
        return "ReadCmdOption";
    case State::readCmdSubOption:
        return "ReadCmdSubOption";
    case State::readEsc:
        return "ReadEsc";
    case State::readEscOption:
        return "ReadEscOption";
    case State::readLine:
        return "ReadLine";
    case State::error:
        return "Error";
    default:
        return "Invalid";
    }
}


} // namespace cli
} // namespace sg
