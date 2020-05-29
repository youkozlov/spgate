#include "gtest/gtest.h"

#include "utils/ParamParser.hpp"

using namespace sg;

void validate(ParamParser const& parser)
{
    ASSERT_EQ(parser.isTtygParsed(), true);
    ASSERT_EQ(parser.isCommonParsed(), true);
    ASSERT_EQ(parser.getNumGates(), 2);
    ASSERT_EQ(parser.getNumDevices(), 2);
    ASSERT_EQ(parser.getNumParams(), 2);

    auto& ttyg = parser.getTtyg();
    EXPECT_EQ(strcmp(ttyg.ipAddr.addr, "0.0.0.0"), 0);
    EXPECT_EQ(ttyg.ipAddr.port, 9999);
    EXPECT_EQ(strcmp(ttyg.serial.serialName, "/dev/ttyS0"), 0);
    EXPECT_EQ(ttyg.serial.block, SerialBlock::off);
    EXPECT_EQ(ttyg.serial.rtscts, SerialRtsCts::off);
    EXPECT_EQ(ttyg.serial.bits, SerialBits::b8);
    EXPECT_EQ(ttyg.serial.stopbits, SerialStopBits::b2);
    EXPECT_EQ(ttyg.serial.parity, SerialParity::none);
    EXPECT_EQ(ttyg.startRxTimeout, 100);
    EXPECT_EQ(ttyg.endRxTimeout, 300);

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
    EXPECT_EQ(gate0.readTimeout, 2000);

    auto& gate1 = parser.getGate(1);
    EXPECT_EQ(gate1.gateType, GateType::m4); 
    EXPECT_EQ(strcmp(gate1.gateAddr.addr, "127.0.0.1"), 0);
    EXPECT_EQ(gate1.gateAddr.port, 8002); 
    EXPECT_EQ(gate1.id, 1);
    EXPECT_EQ(gate1.readPeriod, 1000);
    EXPECT_EQ(gate1.readTimeout, 2000);

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
[ttyg]
ip_addr=0.0.0.0:9999
port=/dev/ttyS0
speed=9600
block=off
rts_cts=off
bits=8
stop_bits=2
parity=none
start_rx_timeout=100
end_rx_timeout=300

[common]
modbus_addr=127.0.0.1:12345

[gate0]
gate_type=sps
gate_addr=127.0.0.1:8001
addr=0x80
request_periodicity=1000
request_timeout=2000

[gate1]
gate_type=m4
gate_addr=127.0.0.1:8002
request_periodicity=1000
request_timeout=2000

[device0]
gate_id=0
addr=0x00

[device1]
gate_id=0
addr=0x01

[param0]
device_id=0
func=0x1d
chan=1
addr=68
type=fixed

[param1]
device_id=1
func=0x1d
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
