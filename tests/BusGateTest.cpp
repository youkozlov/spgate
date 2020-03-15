#include "gtest/gtest.h"
#include "BusGate.hpp"
#include "GateStorage.hpp"
#include "spbus/Buffer.hpp"
#include "spbus/SpBusServer.hpp"
#include "utils/Utils.hpp"

#include "sockets/LinkAcceptorRl.hpp"
#include "utils/Utils.hpp"
#include "types/IpAddr.hpp"

#include "modbus/ModbusClient.hpp"

#include <thread>
#include <atomic>

using namespace sg;

static unsigned int      testPort = 9999;
static std::atomic<bool> done;
static Buffer<float>     spBusBuffer;

static unsigned int      testPortModbus = 12345;
static ModbusStats       clientStats{};
static ModbusBuffer      clientRegisters;

void SpBusServerTest()
{
//    IpAddr const ipAddr = {"192.168.0.193", testPort};
    IpAddr const ipAddr = {"127.0.0.1", testPort};
    LinkAcceptorRl::Init acceptInit = {ipAddr};
    LinkAcceptorRl acceptor(acceptInit);
    SpBusServer::Init spbusInit{spBusBuffer, acceptor};
    SpBusServer server{spbusInit};
    while (!done)
    {        
        server.tickInd();
        Utils::nsleep();
    }
}

void SpBusSendModbusAdu()
{
    ModbusRequest req{1, 0x3, 0, 32};

    IpAddr const ipAddr = {"127.0.0.1", testPortModbus};
    ModbusClient::Init modbusInit{ipAddr, clientRegisters, clientStats};
    ModbusClient client{modbusInit};
    int tick = 0;
    while (!done)
    {        
        tick += 1;
        if (tick % 64 == 0 && client.getState() == ModbusClientState::run)
        {
            client.exec(req);
        }
        client.tickInd();
        Utils::nsleep(1000 * 1000 * 2);
    }
}

TEST(BusGateTest, Init)
{
    char const* testedConfig = "../cfg/default.ini";
    int32_t const testedVal0 = -150000;
    float   const testedVal1 = -153.153;
    float   const testedVal2 =  156.156;
    int32_t const testedVal3 = -1;
    float   const testedVal4 = -0.153;
    float   const testedVal5 =  0.156;

    spBusBuffer[1 * 2048 + 150] = testedVal0;
    spBusBuffer[1 * 2048 + 153] = testedVal1;
    spBusBuffer[1 * 2048 + 156] = testedVal2;
    spBusBuffer[2 * 2048 + 150] = testedVal3;
    spBusBuffer[2 * 2048 + 153] = testedVal4;
    spBusBuffer[2 * 2048 + 156] = testedVal5;

    done = false;
    std::thread thr1(SpBusServerTest);
    std::thread thr2(SpBusSendModbusAdu);

    BusGate::Init spgInit{testedConfig};
    BusGate spgate(spgInit);
    int cnt = 4096;
    while (cnt--)
    {
        spgate.tickInd();
        Utils::nsleep();
    }

    done = true;
    thr1.join();
    thr2.join();

    int32_t rcvdValue0 = Utils::reverse(*((int32_t*)&clientRegisters[0]));
    float   rcvdValue1 = Utils::reverse(*((float*)  &clientRegisters[4]));
    float   rcvdValue2 = Utils::reverse(*((float*)  &clientRegisters[8]));
    int32_t rcvdValue3 = Utils::reverse(*((int32_t*)&clientRegisters[12]));
    float   rcvdValue4 = Utils::reverse(*((float*)  &clientRegisters[16]));
    float   rcvdValue5 = Utils::reverse(*((float*)  &clientRegisters[20]));

    EXPECT_EQ(rcvdValue0, testedVal0);
    EXPECT_EQ(clientRegisters[2], GateReadItemResult::ready);

    EXPECT_NEAR(rcvdValue1, testedVal1, 1e-4);
    EXPECT_EQ(clientRegisters[6], GateReadItemResult::ready);

    EXPECT_NEAR(rcvdValue2, testedVal2, 1e-4);
    EXPECT_EQ(clientRegisters[10], GateReadItemResult::ready);

    EXPECT_EQ(rcvdValue3, testedVal3);
    EXPECT_EQ(clientRegisters[14], GateReadItemResult::ready);

    EXPECT_NEAR(rcvdValue4, testedVal4, 1e-4);
    EXPECT_EQ(clientRegisters[18], GateReadItemResult::ready);

    EXPECT_NEAR(rcvdValue5, testedVal5, 1e-4);
    EXPECT_EQ(clientRegisters[22], GateReadItemResult::ready);

    EXPECT_EQ(clientStats.nTx, clientStats.nRx);
    EXPECT_EQ(0, clientStats.nError);
    EXPECT_EQ(0, clientStats.nInvalid);
}


#if 0
#include "gtest/gtest.h"

#include "mocks/MockTagAccessor.hpp"
#include "mocks/MockSerialPort.hpp"
#include "mocks/MockFeProcessor.hpp"
#include "SpGate.hpp"

using namespace sg;

using ::testing::_;
using ::testing::Return;

void spGateTestInit(SpGate& spg, MockTagAccessor& ta, MockSerialPort& port)
{
    EXPECT_CALL(ta, getType(_))
        .Times(8)
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(1));

    EXPECT_CALL(ta, getShort(_))
        .Times(5)
        .WillRepeatedly(Return(0));

    EXPECT_CALL(port, setup(_))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(ta, setBool(_, _))
        .Times(2);

    EXPECT_CALL(ta, setShort(_, 0))
        .Times(1);
        
    spg.tickInd();
}

void spGateTestRequest(SpGate& spg, MockTagAccessor& ta, MockFeProcessor& fe)
{
    EXPECT_CALL(ta, getBool(_))
        .Times(5)
        .WillRepeatedly(Return(0));

    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    
    EXPECT_CALL(ta, getBool(_))
        .Times(1)
        .WillRepeatedly(Return(1));

    spg.tickInd();
    EXPECT_EQ(spg.getState(), SpGateState::request);

    EXPECT_CALL(ta, getShort(_))
        .Times(6);

    EXPECT_CALL(fe, request(_))
        .Times(1)
        .WillOnce(Return(1));
        
    EXPECT_CALL(ta, setBool(_, _))
        .Times(2);

    EXPECT_CALL(ta, setShort(_, 0))
        .Times(1);

    spg.tickInd();
}

void spGateTestRespond(SpGate& spg, MockTagAccessor& ta, MockFeProcessor& fe)
{
    DataRespond rsp = {};
    rsp.done = false;

    EXPECT_CALL(fe, respond())
        .Times(5)
        .WillRepeatedly(Return(rsp));

    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();

    rsp.done = true;
    rsp.valueType = 3;

    EXPECT_CALL(fe, respond())
        .Times(1)
        .WillOnce(Return(rsp));

    EXPECT_CALL(ta, setBool(_, _))
        .Times(1);

    EXPECT_CALL(ta, setShort(_, _))
        .Times(1);

    EXPECT_CALL(ta, setFloat(_, _))
        .Times(1);
        
    spg.tickInd();
}

void spGateTestDone(SpGate& spg, MockTagAccessor& ta)
{
    EXPECT_CALL(ta, setBool(_, _))
        .Times(5);

    EXPECT_CALL(ta, getBool(_))
        .Times(5)
        .WillRepeatedly(Return(1));
        
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();
    spg.tickInd();

    EXPECT_CALL(ta, setBool(_, _))
        .Times(1);

    EXPECT_CALL(ta, getBool(_))
        .Times(1)
        .WillRepeatedly(Return(0));
      
    spg.tickInd();
}

TEST(SpGateTest, SuccessInit)
{
    MockTagAccessor ta;
    MockSerialPort port;
    MockFeProcessor fe;
    SpGate::Init init{ta, port, fe};
    SpGate spg{init};

    EXPECT_EQ(spg.getState(), SpGateState::init);
    spGateTestInit(spg, ta, port);
    EXPECT_EQ(spg.getState(), SpGateState::idle);
}

TEST(SpGateTest, SuccessRequest)
{
    MockTagAccessor ta;
    MockSerialPort port;
    MockFeProcessor fe;
    SpGate::Init init{ta, port, fe};
    SpGate spg{init};

    EXPECT_EQ(spg.getState(), SpGateState::init);

    spGateTestInit(spg, ta, port);
    EXPECT_EQ(spg.getState(), SpGateState::idle);

    spGateTestRequest(spg, ta, fe);
    EXPECT_EQ(spg.getState(), SpGateState::wait);
}

TEST(SpGateTest, SuccessRespond)
{
    MockTagAccessor ta;
    MockSerialPort port;
    MockFeProcessor fe;
    SpGate::Init init{ta, port, fe};
    SpGate spg{init};

    EXPECT_EQ(spg.getState(), SpGateState::init);

    spGateTestInit(spg, ta, port);
    EXPECT_EQ(spg.getState(), SpGateState::idle);

    spGateTestRequest(spg, ta, fe);
    EXPECT_EQ(spg.getState(), SpGateState::wait);

    spGateTestRespond(spg, ta, fe);
    EXPECT_EQ(spg.getState(), SpGateState::done);    
}

TEST(SpGateTest, SuccessDone)
{
    MockTagAccessor ta;
    MockSerialPort port;
    MockFeProcessor fe;
    SpGate::Init init{ta, port, fe};
    SpGate spg{init};

    EXPECT_EQ(spg.getState(), SpGateState::init);

    spGateTestInit(spg, ta, port);
    EXPECT_EQ(spg.getState(), SpGateState::idle);

    spGateTestRequest(spg, ta, fe);
    EXPECT_EQ(spg.getState(), SpGateState::wait);

    spGateTestRespond(spg, ta, fe);
    EXPECT_EQ(spg.getState(), SpGateState::done);    

    spGateTestDone(spg, ta);
    EXPECT_EQ(spg.getState(), SpGateState::idle);
}
#endif