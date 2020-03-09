#include "gtest/gtest.h"
#include "SpGate.hpp"

using namespace sg;

TEST(SpGateTest, Init)
{
    SpGate::Init spgInit{"../cfg/default.ini"};
    SpGate spgate(spgInit);
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