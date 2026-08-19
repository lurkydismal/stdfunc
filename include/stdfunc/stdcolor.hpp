#pragma once

#include <string_view>

namespace stdfunc::color {

#define COLOR_START "\x1b["

#if defined( STDCOLOR_NO_BOLD )

#define TEXT_FORMATTING "0;"

#else

#define TEXT_FORMATTING "1;"

#endif

#define COLOR_END "m"

constexpr std::string_view g_white = COLOR_START TEXT_FORMATTING "37" COLOR_END;
constexpr std::string_view g_cyanLight =
    COLOR_START TEXT_FORMATTING "36" COLOR_END;
constexpr std::string_view g_blueLight =
    COLOR_START TEXT_FORMATTING "34" COLOR_END;
constexpr std::string_view g_green = COLOR_START TEXT_FORMATTING "32" COLOR_END;
constexpr std::string_view g_purpleLight =
    COLOR_START TEXT_FORMATTING "35" COLOR_END;
constexpr std::string_view g_red = COLOR_START TEXT_FORMATTING "31" COLOR_END;
constexpr std::string_view g_yellow =
    COLOR_START TEXT_FORMATTING "33" COLOR_END;

constexpr std::string_view g_resetForeground = COLOR_START "39" COLOR_END;
constexpr std::string_view g_resetBackground = COLOR_START "49" COLOR_END;
constexpr std::string_view g_reset = COLOR_START "0" COLOR_END;

} // namespace stdfunc::color
