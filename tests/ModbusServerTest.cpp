#include "gtest/gtest.h"

#include "modbus/ModbusClient.hpp"
#include "modbus/ModbusServer.hpp"
#include "modbus/ModbusBuffer.hpp"
#include "sockets/LinkAcceptorRl.hpp"
#include "utils/Utils.hpp"
#include "types/IpAddr.hpp"

#include <thread>
#include <atomic>

using namespace sg;

static unsigned int testPort = 9999;
static std::atomic<bool> done;
static ModbusStats clientStats = {};
static ModbusStats serverStats = {};
static ModbusBuffer clientRegisters;
static ModbusBuffer serverRegisters;

void clearStats()
{
    clientStats = ModbusStats{};
    serverStats = ModbusStats{};
}

void clearRegisters()
{
    clientRegisters.reset();
    serverRegisters.reset();
}

void server()
{
    IpAddr const ipAddr = {"", testPort};
    LinkAcceptorRl::Init acceptInit = {ipAddr};
    std::unique_ptr<LinkAcceptor> acceptor = std::unique_ptr<LinkAcceptorRl>(new LinkAcceptorRl(acceptInit));
    ModbusServer::Init modbusInit{serverRegisters, *acceptor, serverStats};
    ModbusServer server{modbusInit};
    while (!done)
    {        
        server.tickInd();
        Utils::nsleep();
    }
}

void sendModbusAdu(ModbusRequest const& req, int rep)
{
    IpAddr const ipAddr = {"127.0.0.1", testPort};
    ModbusClient::Init modbusInit{ipAddr, clientRegisters, clientStats};
    ModbusClient client{modbusInit};
    int count = 0;
    int tick = 0;
    int timeout = 50 * rep * 10;
    while (!done)
    {        
        tick += 1;
        if (tick % 64 == 0 && client.getState() == ModbusClientState::run)
        {
            client.exec(req);
            count += 1;
            if (count >= rep)
            {
                done = true;
            }
        }
        else if (tick > timeout)
        {
            done = true;
        }
        client.tickInd();
        Utils::nsleep(1000 * 1000 * 2);
    }
}

void sendModbusRequest(ModbusRequest const& req, int rep)
{
    done = false;
    std::thread thr1(server);
    sendModbusAdu(req, rep);
    thr1.join();    
}

//TEST(ModbusServerTest, OnlyServer)
//{
//    testPort = 502;
//    server();
//}

//TEST(ModbusServerTest, OnlyClient)
//{
//    testPort = 502;
//    ModbusRequest req{};
//
//    req = {1, 0x3, 1, 5};
//    sendModbusAdu(req, 50);
//}

TEST(ModbusServerTest, ReadWriteFunctions)
{
    clearStats();
    
    ModbusRequest req{};

    req = {1, 0x3, 1, 32};
    sendModbusRequest(req, 2);

    req = {1, 0x4, 1, 32};
    sendModbusRequest(req, 2);

    req = {1, 0x6, 1, 0};
    sendModbusRequest(req, 2);

    req = {1, 0x10, 1, 32};
    sendModbusRequest(req, 2);

    EXPECT_EQ(clientStats, serverStats);
}

TEST(ModbusServerTest, DataTransferRead)
{
    clearStats();
    clearRegisters();

    for (uint16_t i = 0; i < serverRegisters.size(); ++i)
    {
        serverRegisters[i] = i;
    }

    ModbusRequest req{};

    req = {1, 0x4, 0, 64};
    sendModbusRequest(req, 1);

    for (uint16_t i = req.startReg; i < (req.startReg + req.numRegs); ++i)
    {
        EXPECT_EQ(serverRegisters[i], clientRegisters[i]) << i;
    }

    EXPECT_EQ(clientStats, serverStats);
}

TEST(ModbusServerTest, DataTransferWrite)
{
    clearStats();
    clearRegisters();

    for (uint16_t i = 0; i < clientRegisters.size(); ++i)
    {
        clientRegisters[i] = i;
    }

    ModbusRequest req{};

    req = {1, 0x10, 64, 64};
    sendModbusRequest(req, 1);

    for (uint16_t i = req.startReg; i < (req.startReg + req.numRegs); ++i)
    {
        EXPECT_EQ(serverRegisters[i], clientRegisters[i]) << i;
    }

    EXPECT_EQ(clientStats, serverStats);
}

TEST(ModbusServerTest, ErrorCase)
{
    clearStats();
    
    ModbusRequest req{};

    req = {1, 0x3, 20000, 32};
    sendModbusRequest(req, 5);

    req = {1, 0x4, 1, 32000};
    sendModbusRequest(req, 5);

    EXPECT_EQ(10, serverStats.nError);
}
