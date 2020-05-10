#pragma once

namespace sg
{
namespace cli
{

constexpr unsigned IAC  = 255;
constexpr unsigned DONT = 254;
constexpr unsigned DO   = 253;
constexpr unsigned WONT = 252;
constexpr unsigned WILL = 251;
constexpr unsigned SB   = 250;
constexpr unsigned GA   = 249;
constexpr unsigned SE   = 240;


constexpr unsigned OPT_ECHO = 1;
constexpr unsigned OPT_GA   = 3;
constexpr unsigned OPT_NAWS = 31;
constexpr unsigned OPT_LM   = 34;

} // namespace cli
} // namespace sg
