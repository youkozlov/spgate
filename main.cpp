#include "shm.hpp"
#include <rlserial.h>

#include <memory>
#include "app/sockets/LinkAcceptorRl.hpp"
#include "app/modbus/ModbusBuffer.hpp"
#include "app/modbus/ModbusServer.hpp"
#include "app/utils/ParamParser.hpp"
#include "app/SerialPortRl.hpp"
#include "app/TagAccessorSl.hpp"
#include "app/FeProcessorSpNet.hpp"
#include "app/FeProcessorM4Net.hpp"
#include "app/SpGate.hpp"

int main()
{   
 
    rlSerial serial;
    Shm mymem("./load_files.srv");

    if (mymem.getType("spgDeviceNet") != SHORT)
    {
        exit(1);
    }
    
    struct timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = 1000000; // 1мс
    nanosleep(&tv, NULL);

    sg::ParamParser paramParser;
    paramParser.parseFile("cfg/default.ini");
    
    sg::LinkAcceptorRl::Init accInit{9999};
    sg::LinkAcceptorRl acceptor{accInit};
    
    sg::ModbusStats stats;
    sg::ModbusBuffer registers;
    sg::ModbusServer::Init mbInit{registers, acceptor, stats};
    sg::ModbusServer mb{mbInit};
    
    sg::SerialPortRl port{serial};
    sg::TagAccessorSl ta{mymem};
    std::unique_ptr<sg::FeProcessor> fe;
    
    switch(ta.getShort("spgDeviceNet"))
    {
    case 0:
    {
        sg::FeProcessorSpNet::Init init{port};
        fe = std::unique_ptr<sg::FeProcessor>(new sg::FeProcessorSpNet(init));
    }
    break;
    default:
    {
        sg::FeProcessorM4Net::Init init{port};
        fe = std::unique_ptr<sg::FeProcessor>(new sg::FeProcessorM4Net(init));
    }
    break;
    }
    
    sg::SpGate::Init init{ta, port, *fe};
    sg::SpGate spg{init};
    
    spg.tickInd();
    
    if (spg.getState() != sg::SpGateState::idle)
    {
        exit(1);
    }
    
    while (1)
    {
        nanosleep(&tv, NULL);
        mb.tickInd();
        spg.tickInd();
    }
}