#pragma once

#include "types/IpAddr.hpp"
#include <memory>

namespace sg
{

class LinkAcceptor;
class BusGate;
class WrapBuffer;
struct ModbusStats;
struct BusStats;

namespace cli
{

class CmdProcessor;
class TelnetServer;

class Cli
{
public:
    struct Init
    {
        sg::IpAddr& telnetAddr;
        BusGate&    bg;
    };

    explicit Cli(Init const&);

    ~Cli();

    void tickInd();

    int receive(unsigned char*, unsigned int, WrapBuffer&);

    void exit();

    void shutdown();

    void error();

    ModbusStats const& getModbusStats() const;

    BusStats const& getSpBusStats() const;

    BusStats const& getRsBusStats() const;

private:
    BusGate&                      bg;
    std::unique_ptr<CmdProcessor> cmdProc;
    std::unique_ptr<LinkAcceptor> linkAcceptor;
    std::unique_ptr<TelnetServer> telnet;
};

} // namespace cli
} // namespace sg
