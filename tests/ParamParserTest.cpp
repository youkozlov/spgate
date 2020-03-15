#include "gtest/gtest.h"

#include "utils/ParamParser.hpp"

using namespace sg;

void validate(ParamParser const& parser)
{
    EXPECT_EQ(parser.getNumGates(), 2);
    EXPECT_EQ(parser.getNumDevices(), 2);
    EXPECT_EQ(parser.getNumParams(), 2);
    
    auto& common = parser.getCommon();
    EXPECT_EQ(strcmp(common.modbusAddr.addr, "127.0.0.1"), 0);
    EXPECT_EQ(strlen(common.modbusAddr.addr), strlen("127.0.0.1")); 
    EXPECT_EQ(common.modbusAddr.port, 12345); 

    auto& gate0 = parser.getGate(0);
    EXPECT_EQ(gate0.gateType, GateType::sps); 
    EXPECT_EQ(strcmp(gate0.gateAddr.addr, "127.0.0.1"), 0);
    EXPECT_EQ(gate0.gateAddr.port, 8001); 
    EXPECT_EQ(gate0.addr, 0x80); 
    EXPECT_EQ(gate0.id, 0); 
    EXPECT_EQ(gate0.readPeriod, 1000); 

    auto& gate1 = parser.getGate(1);
    EXPECT_EQ(gate1.gateType, GateType::m4); 
    EXPECT_EQ(strcmp(gate1.gateAddr.addr, "127.0.0.1"), 0);
    EXPECT_EQ(gate1.gateAddr.port, 8002); 
    EXPECT_EQ(gate1.id, 1);
    EXPECT_EQ(gate1.readPeriod, 1000); 

    auto& device0 = parser.getDevice(0);
    EXPECT_EQ(device0.gateId, 0); 
    EXPECT_EQ(device0.addr, 0); 
    EXPECT_EQ(device0.id, 0);

    auto& param1 = parser.getParam(1);
    EXPECT_EQ(param1.deviceId, 1); 
    EXPECT_EQ(param1.func, 0x1d); 
    EXPECT_EQ(param1.chan, 1); 
    EXPECT_EQ(param1.addr, 67); 
    EXPECT_EQ(param1.id, 1);
    EXPECT_EQ(param1.type, ParamType::floatPoint);
}

TEST(ParamParserTest, ParseString)
{
    std::string data = 
R"foo(
[common]
modbus_addr=127.0.0.1:12345

[gate0]
gate_type=sps
gate_addr=127.0.0.1:8001
addr=80
read_period=1000

[gate1]
gate_type=m4
gate_addr=127.0.0.1:8002
read_period=1000

[device0]
gate_id=0
addr=00

[device1]
gate_id=0
addr=01

[param0]
device_id=0
func=1d
chan=1
addr=68
type=fixed

[param1]
device_id=1
func=1d
chan=1
addr=67
type=float
)foo";

    ParamParser parser;
    ASSERT_TRUE(parser.parseString(data));
    validate(parser);
}

TEST(ParamParserTest, ParseFile)
{
    ParamParser parser;
    EXPECT_TRUE(parser.parseFile("../cfg/default_ut.ini"));
    validate(parser);
}
