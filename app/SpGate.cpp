#include "SpGate.hpp"

#include "sockets/LinkAcceptorRl.hpp"
#include "modbus/ModbusServer.hpp"

#include "gates/GateSpBus.hpp"
#include "gates/GateM4Bus.hpp"

#include "utils/Logger.hpp"

namespace sg
{

SpGate::SpGate(Init const& init)
    : iniFileName(init.iniFileName)
    , state(SpGateState::init)
{
}

SpGate::~SpGate()
{
}

void SpGate::tickInd()
{
    switch (state)
    {
    case SpGateState::init:
        processInit();
    return;
    case SpGateState::run:
        processRun();
    return;
    case SpGateState::error:
        processError();
    return;
    }
}

void SpGate::processInit()
{
    if (!parser.parseFile(iniFileName))
    {
        chageState(SpGateState::error);
        return;
    }

    if (!createModbus())
    {
        chageState(SpGateState::error);
        return;
    }

    if (!createGates())
    {
        chageState(SpGateState::error);
        return;
    }

    chageState(SpGateState::run);
}

void SpGate::processRun()
{
    for (auto& gate : gates)
    {
        if (!gate) break;
        gate->tickInd();
    }
    modbus->tickInd();
}

void SpGate::processError()
{
}

bool SpGate::createModbus()
{
    auto& common = parser.getCommon();
    
    LinkAcceptorRl::Init acceptInit = {common.modbusAddr};
    linkAcceptor = std::unique_ptr<LinkAcceptorRl>(new LinkAcceptorRl(acceptInit));

    ModbusServer::Init modbusInit{modbusRegs, *linkAcceptor, modbusStats};
    modbus = std::unique_ptr<ModbusServer>(new ModbusServer(modbusInit));

    return true;
}

bool SpGate::createGates()
{
    if (parser.getNumGates() > maxNumGates || !parser.getNumGates())
    {
        LM(LE, "Unexpected number of configured gates=%u", parser.getNumGates());
        return false;
    }

    for (unsigned int i = 0; i < parser.getNumGates(); ++i)
    {
        auto& it = parser.getGate(i);

        switch (it.gateType)
        {
        case GateType::sps:
        {
            GateSpBus::Init init{it, parser, modbusRegs};
            gates[i] = std::unique_ptr<Gate>(new GateSpBus(init));
        }
        break;
        case GateType::m4:
        {
            GateM4Bus::Init init{it, parser, modbusRegs};
            gates[i] = std::unique_ptr<Gate>(new GateM4Bus(init));
        }
        break;
        }

        if (!gates[i]->configure())
        {
            LM(LE, "Can't configure gate=%u", i);
            return false;
        }
    }

    return true;
}

void SpGate::chageState(SpGateState newSt)
{
    LM(LI, "Change state: %s -> %s", toString(state), toString(newSt));
    state = newSt;
}

SpGateState SpGate::getState() const
{
    return state;
}

char const* SpGate::toString(SpGateState st) const
{
    switch (st)
    {
    case SpGateState::init:
        return "Init";
    case SpGateState::run:
        return "Run";
    case SpGateState::error:
        return "Error";
    default:
        return "Invalid";
    }
}

}
