#pragma once

#include "stdfunc.hpp"

constexpr size_t g_floaT16TypeBitAmount = 16;

using float16_t = _Float16 __attribute__( (
    aligned( stdfunc::bitsToBytes( g_floaT16TypeBitAmount ) ) ) );

using float16x2_t = float16_t __attribute__( (
    vector_size( stdfunc::bitsToBytes( g_floaT16TypeBitAmount * 2 ) ) ) );
using float16x4_t = float16_t __attribute__( (
    vector_size( stdfunc::bitsToBytes( g_floaT16TypeBitAmount * 4 ) ) ) );
using float16x8_t = float16_t __attribute__( (
    vector_size( stdfunc::bitsToBytes( g_floaT16TypeBitAmount * 8 ) ) ) );
using float16x16_t = float16_t __attribute__( (
    vector_size( stdfunc::bitsToBytes( g_floaT16TypeBitAmount * 16 ) ) ) );
using float16x32_t = float16_t __attribute__( (
    vector_size( stdfunc::bitsToBytes( g_floaT16TypeBitAmount * 32 ) ) ) );

namespace std {

template <>
struct numeric_limits< float16_t > {
    static constexpr bool is_specialized = true;

    static constexpr auto min() -> float16_t { return ( 6.103515625e-5f ); }

    static constexpr auto max() -> float16_t { return ( 65504.0f ); }

    static constexpr auto lowest() -> float16_t { return ( -65504.0f ); }

    static constexpr auto epsilon() -> float16_t { return ( 0.0009765625f ); }

    static constexpr auto round_error() -> float16_t { return 0.5f; }

    static constexpr auto infinity() -> float16_t {
        return ( float16_t{ INFINITY } );
    }

    static constexpr auto quiet_NaN() -> float16_t {
        return ( float16_t{ NAN } );
    }

    static constexpr auto signaling_NaN() -> float16_t {
        return ( float16_t{ NAN } );
    }

    static constexpr auto denorm_min() -> float16_t {
        return ( 5.960464477539063e-8f );
    }

    static constexpr int digits = 11;      // Mantissa bits
    static constexpr int digits10 = 3;     // Base-10 precision
    static constexpr int max_digits10 = 5; // For round-trip conversion

    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;
    static constexpr float_round_style round_style = round_to_nearest;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr bool traps = true;
    static constexpr bool tinyness_before = false;
    static constexpr float_denorm_style has_denorm = denorm_present;
};

} // namespace std

template <>
struct std::formatter< float16_t, char > {
    constexpr auto parse( std::format_parse_context& _context ) {
        return ( _context.begin() );
    }

    auto format( float16_t _value, std::format_context& _context ) const {
        return ( std::format_to( _context.out(), "{{}}", _value ) );
    }
};
