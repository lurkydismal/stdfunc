#pragma once

#include <string_view>

namespace stdfunc::color {

constexpr std::string_view g_white = "\x1b[1;37m";
constexpr std::string_view g_cyanLight = "\x1b[1;36m";
constexpr std::string_view g_blueLight = "\x1b[1;34m";
constexpr std::string_view g_green = "\x1b[1;32m";
constexpr std::string_view g_purpleLight = "\x1b[1;35m";
constexpr std::string_view g_red = "\x1b[1;31m";
constexpr std::string_view g_yellow = "\x1b[1;33m";
constexpr std::string_view g_resetForeground = "\x1b[39m";
constexpr std::string_view g_resetBackground = "\x1b[49m";
constexpr std::string_view g_reset = "\x1b[0m";

} // namespace stdfunc::color
