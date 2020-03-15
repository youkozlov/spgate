#include "BusGate.hpp"

#include "sockets/LinkAcceptorRl.hpp"
#include "modbus/ModbusServer.hpp"

#include "spbus/SpBusClient.hpp"
#include "rsbus/RsBusClient.hpp"

#include "utils/Logger.hpp"

namespace sg
{

BusGate::BusGate(Init const& init)
    : iniFileName(init.iniFileName)
    , state(BusGateState::init)
{
}

BusGate::~BusGate()
{
}

void BusGate::tickInd()
{
    switch (state)
    {
    case BusGateState::init:
        processInit();
    return;
    case BusGateState::run:
        processRun();
    return;
    case BusGateState::error:
        processError();
    return;
    }
}

void BusGate::processInit()
{
    if (!parser.parseFile(iniFileName))
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
    modbus->tickInd();
}

void BusGate::processError()
{
}

bool BusGate::createModbus()
{
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

    for (unsigned int i = 0; i < parser.getNumGates(); ++i)
    {
        auto& it = parser.getGate(i);

        try
        {
            switch (it.gateType)
            {
            case GateType::sps:
            {
                SpBusClient::Init init{it, parser, modbusRegs};
                gates[i] = std::unique_ptr<Client>(new SpBusClient(init));
            }
            break;
            case GateType::m4:
            {
                RsBusClient::Init init{it, parser, modbusRegs};
                gates[i] = std::unique_ptr<Client>(new RsBusClient(init));
            }
            break;
            }
        }
        catch(char const*)
        {
            LM(LE, "Can't configure gate=%u", i);
            return false;
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
    default:
        return "Invalid";
    }
}

}
