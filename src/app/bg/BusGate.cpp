#include "BusGate.hpp"

#include "cli/Cli.hpp"

#include "sockets/LinkAcceptorRl.hpp"
#include "modbus/ModbusServer.hpp"

#include "spbus/SpBusServer.hpp"
#include "spbus/SpBusClient.hpp"
#include "rsbus/RsBusServer.hpp"
#include "rsbus/RsBusClient.hpp"

#include "utils/Logger.hpp"

namespace sg
{

BusGate::BusGate(Init const& init)
    : iniFileName(init.iniFileName)
    , state(BusGateState::init)
    , regAccessor(modbusRegs)
    , cliPort(init.cliPort)
    , isEmulMode(init.isEmulMode)
{
    static_assert(sizeof(int) == 4, "");
    static_assert(sizeof(float) == 4, "");
    static_assert(sizeof(unsigned) == 4, "");
}

BusGate::~BusGate()
{
}

int BusGate::tickInd()
{
    switch (state)
    {
    case BusGateState::init:
        processInit();
    return 0;
    case BusGateState::run:
        processRun();
    return 0;
    case BusGateState::error:
        processError();
    return 0;
    case BusGateState::shutdown:
    return 1;
    }
    return 1;
}

void BusGate::shutdown()
{
    chageState(BusGateState::shutdown);
}

void BusGate::processInit()
{
    if (!parser.parseFile(iniFileName))
    {
        chageState(BusGateState::error);
        return;
    }

    if (!parser.isCommonParsed())
    {
        LM(LE, "Can't find common section");
        chageState(BusGateState::error);
        return;
    }

    if (!createCli())
    {
        chageState(BusGateState::error);
        return;
    }

    if (!createModbus())
    {
        chageState(BusGateState::error);
        return;
    }

    if (!createGates())
    {
        chageState(BusGateState::error);
        return;
    }

    chageState(BusGateState::run);
}

void BusGate::processRun()
{
    for (auto& gate : gates)
    {
        if (!gate) break;
        gate->tickInd();
    }

    if (modbus)
    {
        modbus->tickInd();
    }

    if (cli)
    {
        cli->tickInd();
    }
}

void BusGate::processError()
{
}

bool BusGate::createCli()
{
    if (!cliPort)
    {
        return true;
    }

    IpAddr ipAddr{"127.0.0.1", cliPort};
    cli::Cli::Init init{ipAddr, *this};
    cli = std::unique_ptr<cli::Cli>(new cli::Cli(init));

    return true;
}

bool BusGate::createModbus()
{
    if (isEmulMode)
    {
        return true;
    }

    auto& common = parser.getCommon();
    
    LinkAcceptorRl::Init acceptInit = {common.modbusAddr};
    linkAcceptor = std::unique_ptr<LinkAcceptorRl>(new LinkAcceptorRl(acceptInit));

    ModbusServer::Init modbusInit{modbusRegs, *linkAcceptor, modbusStats};
    modbus = std::unique_ptr<ModbusServer>(new ModbusServer(modbusInit));

    return true;
}

bool BusGate::createGates()
{
    if (parser.getNumGates() > maxNumGates || !parser.getNumGates())
    {
        LM(LE, "Unexpected number of configured gates=%u", parser.getNumGates());
        return false;
    }

    if (isEmulMode)
    {
        return createGatesServer();
    }
    return createGatesClient();
}

bool BusGate::createGatesClient()
{
    for (unsigned int i = 0; i < parser.getNumGates(); ++i)
    {
        sg::GateParams const& gatePrms = parser.getGate(i);

        switch (gatePrms.gateType)
        {
        case GateType::sps:
        {
            SpBusClient::Init init{gatePrms, parser, regAccessor, spbusStats, linkLocker};
            gates[i] = std::unique_ptr<Bus>(new SpBusClient(init));
        }
        break;
        case GateType::m4:
        {
            rsbus::RsBusClient::Init init{gatePrms, parser, regAccessor, rsbusStats, linkLocker};
            gates[i] = std::unique_ptr<Bus>(new rsbus::RsBusClient(init));
        }
        break;
        }
    }

    return true;
}

bool BusGate::createGatesServer()
{
    serverRegs = std::unique_ptr<Buffer<float>>(new Buffer<float>());

    for (unsigned int i = 0; i < parser.getNumGates(); ++i)
    {
        sg::GateParams const& gatePrms = parser.getGate(i);

        switch (gatePrms.gateType)
        {
        case GateType::sps:
        {
            SpBusServer::Init init{*serverRegs, gatePrms.gateAddr};
            gates[i] = std::unique_ptr<Bus>(new SpBusServer(init));
        }
        break;
        case GateType::m4:
        {
            rsbus::RsBusServer::Init init{*serverRegs, gatePrms.gateAddr};
            gates[i] = std::unique_ptr<Bus>(new rsbus::RsBusServer(init));
        }
        break;
        }
    }

    return true;
}

void BusGate::chageState(BusGateState newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

BusGateState BusGate::getState() const
{
    return state;
}

ModbusStats const& BusGate::getModbusStats() const
{
    return modbusStats;
}

BusStats const& BusGate::getSpBusStats() const
{
    return spbusStats;
}

BusStats const& BusGate::getRsBusStats() const
{
    return rsbusStats;
}

char const* BusGate::toString(BusGateState st) const
{
    switch (st)
    {
    case BusGateState::init:
        return "Init";
    case BusGateState::run:
        return "Run";
    case BusGateState::error:
        return "Error";
    case BusGateState::shutdown:
        return "Shutdown";
    default:
        return "Invalid";
    }
}

}
