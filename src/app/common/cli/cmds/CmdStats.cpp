#include "CmdStats.hpp"
#include "CmdArgStatsType.hpp"
#include "CmdParamParser.hpp"
#include "CmdStatsFormater.hpp"
#include "cli/Cli.hpp"
#include "bus/modbus/ModbusDefs.hpp"
#include "bus/BusStats.hpp"
#include "utils/WrapBuffer.hpp"
#include "CmdDefs.hpp"

namespace sg
{
namespace cli
{

void fillModbusStats(ModbusStats const& modbusStats, StatsItems& stats)
{
    std::strcpy(stats.name, "Statistics modbus");

    Item* item = stats.next();
    std::strcpy(item->name, "nRx");
    item->value = modbusStats.nRx;

    item = stats.next();
    std::strcpy(item->name, "nTx");
    item->value = modbusStats.nTx;

    item = stats.next();
    std::strcpy(item->name, "nInvalid");
    item->value = modbusStats.nInvalid;

    item = stats.next();
    std::strcpy(item->name, "nError");
    item->value = modbusStats.nError;

    item = stats.next();
    std::strcpy(item->name, "nRd");
    item->value = modbusStats.nRd;

    item = stats.next();
    std::strcpy(item->name, "nWr");
    item->value = modbusStats.nWr;

    item = stats.next();
    std::strcpy(item->name, "nMultiWr");
    item->value = modbusStats.nMultiWr;
}

void fillBusStats(char const* name, BusStats const& busStats, StatsItems& stats)
{
    std::strcpy(stats.name, name);

    Item* item = stats.next();
    std::strcpy(item->name, "nRx");
    item->value = busStats.nRx;

    item = stats.next();
    std::strcpy(item->name, "nTx");
    item->value = busStats.nTx;

    item = stats.next();
    std::strcpy(item->name, "nInvalid");
    item->value = busStats.nInvalid;

    item = stats.next();
    std::strcpy(item->name, "nError");
    item->value = busStats.nError;

    item = stats.next();
    std::strcpy(item->name, "nRdp");
    item->value = busStats.nRdp;

    item = stats.next();
    std::strcpy(item->name, "nRsp");
    item->value = busStats.nRsp;
}

CmdStats::CmdStats(Cli& cli_)
    : cli(cli_)
{
    args.push_back(std::unique_ptr<CmdArg>(new CmdArgStatsType));
}

CmdStats::~CmdStats()
{
}

void CmdStats::execute(CmdParamParser const& prms, WrapBuffer& txBuf)
{
    CmdStatsFormater formater(*this, txBuf);
    if (!prms.numArgs())
    {
        StatsItems stats;
        fillModbusStats(cli.getModbusStats(), stats);
        formater.display(stats);
    }
    else
    {
        for (unsigned int i = 0; i < prms.numArgs(); ++i)
        {
            auto res = std::find_if(args.cbegin(), args.cend(), [i, &prms](std::unique_ptr<CmdArg> const& it)
            {
                return strncmp(it->name(), prms.arg(i).name, maxLengthName) == 0;
            });
            if (res == args.cend())
            {
                txBuf.write(invalidCommandArguments, strlen(invalidCommandArguments));
                return;
            }
        }
        for (unsigned int i = 0; i < args.size(); ++i)
        {
            CmdArg& item = *args[i];
            int const id = prms.find(item.name(), true);
            if (id != -1)
            {
                char const* prmType = prms.arg(id).param;
                if (0 == std::strncmp(prmType, "modbus", maxLengthName))
                {
                    StatsItems stats;
                    fillModbusStats(cli.getModbusStats(), stats);
                    formater.display(stats);
                    return;
                }
                else if (0 == std::strncmp(prmType, "spbus", maxLengthName))
                {
                    StatsItems stats;
                    fillBusStats("Statistics spbus", cli.getSpBusStats(), stats);
                    formater.display(stats);
                    return;
                }
                else if (0 == std::strncmp(prmType, "rsbus", maxLengthName))
                {
                    StatsItems stats;
                    fillBusStats("Statistics rsbus", cli.getRsBusStats(), stats);
                    formater.display(stats);
                    return;
                }
                txBuf.write(invalidCommandArguments, strlen(invalidCommandArguments));
                return;
            }
        }
    }
}

char const* CmdStats::getName()
{
    return "stats";
}

} // namespace cli
} // namespace sg
