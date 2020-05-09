#include "CmdLog.hpp"
#include "CmdArgLogLevel.hpp"
#include "utils/WrapBuffer.hpp"
#include "CmdParamFormater.hpp"
#include "CmdParamParser.hpp"
#include <sstream>

namespace sg
{
namespace cli
{

char const* invalidCommandArguments = "\r\n?Invalid command arguments\r\n";

CmdLog::CmdLog()
{
    args.push_back(std::unique_ptr<CmdArg>(new CmdArgLogLevel));
}

CmdLog::~CmdLog()
{
}

char const* CmdLog::getName()
{
    return "log";
}

void CmdLog::execute(CmdParamParser const& prms, WrapBuffer& txBuf)
{
    if (!prms.numArgs())
    {
        CmdParamFormater formater(*this, txBuf);
        formater.display(args);
    }
    else
    {
        for (unsigned int i = 0; i < prms.numArgs(); ++i)
        {
            auto res = std::find_if(args.cbegin(), args.cend(), [i, &prms](std::unique_ptr<CmdArg> const& it)
            {
                return strcmp(it->name(), prms.arg(i).name) == 0;
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
                if (!item.setValue(prms.arg(id).param))
                {
                    txBuf.write(invalidCommandArguments, strlen(invalidCommandArguments));
                    return;
                }
            }
        }
    }
}

} // namespace cli
} // namespace sg
