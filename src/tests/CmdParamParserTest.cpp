#include "gtest/gtest.h"

#include "cli/cmds/CmdParamParser.hpp"

using namespace sg;

TEST(CmdParamParserTest, ParseName)
{
    char const* logLevelCmd = "log level";
    char buf[256] = {};
    memcpy(buf, logLevelCmd, strlen(logLevelCmd));
    cli::CmdParamParser parser(buf, strlen(logLevelCmd));

    ASSERT_EQ(parser.parse(), true);
    ASSERT_EQ(parser.numArgs(), 1);

    EXPECT_EQ(strcmp(parser.cmdName(), "log"), 0);
    EXPECT_EQ(strcmp(parser.arg(0).name, "level"), 0);
    EXPECT_EQ(parser.arg(0).param, nullptr);
}

TEST(CmdParamParserTest, ParseNameParam)
{
    char const* logLevelCmd = "log level=0";
    char buf[256] = {};
    memcpy(buf, logLevelCmd, strlen(logLevelCmd));
    cli::CmdParamParser parser(buf, strlen(logLevelCmd));

    ASSERT_EQ(parser.parse(), true);
    ASSERT_EQ(parser.numArgs(), 1);

    EXPECT_EQ(strcmp(parser.cmdName(), "log"), 0);
    EXPECT_EQ(strcmp(parser.arg(0).name, "level"), 0);
    EXPECT_EQ(strcmp(parser.arg(0).param, "0"), 0);
}