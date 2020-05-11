#pragma once

#include <array>

namespace sg
{

class WrapBuffer;
class Link;

namespace cli
{

class TelnetServer;

class TelnetPrinter
{
public:
    enum class State
    {
        readLine,
        readCmd,
        readCmdOption,
        readCmdSubOption,
        readEsc,
        readEscOption,
        error
    };

    explicit TelnetPrinter(Link&, TelnetServer&);

    ~TelnetPrinter();

    int init();

    int process();

private:
    int receive();

    void readLine(unsigned char);
    void readCmd(unsigned char);
    void readCmdOption(unsigned char);
    void readCmdSubOption(unsigned char);
    void readEsc(unsigned char);
    void readEscOption(unsigned char);

    void changeState(State);
    char const* toString(State) const;

    State         state;
    Link&         link;
    TelnetServer& srv;
    unsigned int                    lineLen;
    std::array<unsigned char, 1024> line;

    bool optGaEnabled;
    bool optEchoEnabled;
    bool optLmEnabled;

    unsigned char cmd;

    std::array<unsigned char, 1024> rawBuffer;
    
    std::array<unsigned char, 1024> lastCmd;
    uint32_t                        lastCmdLen;
};

} // namespace cli
} // namespace sg
