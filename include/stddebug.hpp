#pragma once

#include <format>

#if defined( DEBUG )

#include <cstddef>
#include <iostream>
#include <print>
#include <stacktrace>
#include <string>
#include <string_view>
#include <thread>

#include "stdcolor.hpp"

#endif

#include "stdconcepts.hpp"

#if defined( assert )

#undef assert

#endif

// Debug utility functions ( side-effects )
namespace stdfunc {

#if defined( DEBUG )

namespace {

constexpr std::string_view g_trapColorLevel = color::g_red;
constexpr std::string_view g_trapColorThreadId = color::g_purpleLight;
constexpr std::string_view g_trapColorFileName = color::g_purpleLight;
constexpr std::string_view g_trapColorLineNumber = color::g_purpleLight;
constexpr std::string_view g_trapColorFunctionName = color::g_purpleLight;

constexpr size_t g_backtraceLimit = 10;

constexpr auto formatWithColor( auto _what, std::string_view _color )
    -> std::string {
    return ( std::format( "{}{}{}", _color, _what,
                          stdfunc::color::g_resetForeground ) );
}

} // namespace

template < is_formattable... Arguments >
[[noreturn]] constexpr void trap(
    std::format_string< Arguments... > _format = "",
    Arguments&&... _arguments ) {
    if !consteval {
        std::print(
            std::cerr, "{} Thread {}{:#X}{}:",
            formatWithColor( "[TRAP]", g_trapColorLevel ), g_trapColorThreadId,
            std::hash< std::thread::id >{}( std::this_thread::get_id() ),
            color::g_reset );
        std::println( std::cerr, _format,
                      std::forward< Arguments >( _arguments )... );
        // FIX: No stacktrace
        std::println(
            "{}{}",
            formatWithColor( std::stacktrace::current( 1, g_backtraceLimit ),
                             color::g_white ),
            color::g_reset );
    }

    __builtin_trap();
}

template < is_formattable... Arguments >
constexpr void assert( bool _result,
                       std::format_string< Arguments... > _format = "",
                       Arguments&&... _arguments ) {
    if ( !_result ) [[unlikely]] {
        trap( _format, std::forward< Arguments >( _arguments )... );
    }
}

#else

template < is_formattable... Arguments >
constexpr void trap(
    [[maybe_unused]] std::format_string< Arguments... > _format = "",
    [[maybe_unused]] Arguments&&... _arguments ) {}

template < is_formattable... Arguments >
constexpr void assert(
    [[maybe_unused]] bool _result,
    [[maybe_unused]] std::format_string< Arguments... > _format = "",
    [[maybe_unused]] Arguments&&... _arguments ) {}

#endif

} // namespace stdfunc
