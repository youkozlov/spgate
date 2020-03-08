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
    EXPECT_EQ(common.readPeriod, 1000); 

    auto& gate1 = parser.getGate(0);
    EXPECT_EQ(gate1.gateType, GateType::sps); 
    EXPECT_EQ(strcmp(gate1.gateAddr.addr, "127.0.0.1"), 0);
    EXPECT_EQ(gate1.gateAddr.port, 8001); 
    EXPECT_EQ(gate1.addr, 00); 

    auto& gate2 = parser.getGate(1);
    EXPECT_EQ(gate2.gateType, GateType::m4); 
    EXPECT_EQ(strcmp(gate2.gateAddr.addr, "127.0.0.1"), 0);
    EXPECT_EQ(gate2.gateAddr.port, 8002); 

    auto& device1 = parser.getDevice(0);
    EXPECT_EQ(device1.gateId, 1); 
    EXPECT_EQ(device1.addr, 0); 

    auto& param2 = parser.getParam(1);
    EXPECT_EQ(param2.deviceId, 2); 
    EXPECT_EQ(param2.func, 0x1d); 
    EXPECT_EQ(param2.chan, 1); 
    EXPECT_EQ(param2.addr, 67); 
}

TEST(ParamParserTest, ParseString)
{
    std::string data = 
R"foo([common]
modbus_addr=127.0.0.1:12345
read_period=1000
[gate1]
gate_type=sps
gate_addr=127.0.0.1:8001
addr=00
[gate2]
gate_type=m4
gate_addr=127.0.0.1:8002
[device1]
gate_id=1
addr=00
[device2]
gate_id=1
addr=01
[param1]
device_id=1
func=1d
chan=1
addr=68
[param2]
device_id=2
func=1d
chan=1
addr=67
)foo";

    ParamParser parser;
    EXPECT_TRUE(parser.parseString(data));
    validate(parser);
}

TEST(ParamParserTest, ParseFile)
{
    ParamParser parser;
    EXPECT_TRUE(parser.parseFile("../cfg/default.ini"));
    validate(parser);
}
