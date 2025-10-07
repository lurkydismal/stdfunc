#pragma once

#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <xxh3.h>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <gsl/pointers>
#include <random>
#include <ranges>
#include <regex>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ctll.hpp"
#include "ctre.hpp"

#if defined( DEBUG )

#include <iostream>
#include <print>
#include <stacktrace>
#include <thread>

#endif

// Function attributes
#define FORCE_INLINE [[gnu::always_inline]] inline
#define NOINLINE [[gnu::noinline, gnu::used]]
#define CONST [[gnu::const]]
#define PURE [[gnu::pure]]
#define HOT [[gnu::hot]]
#define COLD [[gnu::cold]]
#define SENTINEL [[gnu::sentinel]]
#define CONSTRUCTOR [[gnu::constructor]]
#define DESTRUCTOR [[gnu::destructor]]
#define EXPORT extern "C"

// Struct attributes
#define PACKED [[gnu::packed]]

using uint128_t = __uint128_t;

namespace stdfunc {

// TODO: Add support for 64bits
namespace system {

using call_t = enum class call : uint8_t {
    exit = 1,
    read = 3,
    write = 4,
    ioctl = 54,
};

using callUnderlying_t = std::underlying_type_t< call_t >;

template < typename... Arguments >
    requires( sizeof...( Arguments ) <= 3 &&
              ( std::convertible_to< Arguments, uintptr_t > && ... ) )
FORCE_INLINE void syscall( call_t _systemCall, Arguments... _arguments ) {
    const auto l_systemCall = static_cast< uint32_t >( _systemCall );

    constexpr size_t l_amount = sizeof...( Arguments );

    std::array l_arguments = { static_cast< uintptr_t >( _arguments )... };

    if constexpr ( l_amount == 0 ) {
        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall )
                          : "memory", "cc" );

    } else if constexpr ( l_amount == 1 ) {
        const uintptr_t l_ebx = l_arguments[ 0 ];

        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall ), "b"( l_ebx )
                          : "memory", "cc" );

    } else if constexpr ( l_amount == 2 ) {
        const uintptr_t l_ebx = l_arguments[ 0 ];
        const uintptr_t l_ecx = l_arguments[ 1 ];

        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall ), "b"( l_ebx ), "c"( l_ecx )
                          : "memory", "cc" );

    } else if constexpr ( l_amount == 3 ) {
        const uintptr_t l_ebx = l_arguments[ 0 ];
        const uintptr_t l_ecx = l_arguments[ 1 ];
        const uintptr_t l_edx = l_arguments[ 2 ];

        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall ), "b"( l_ebx ), "c"( l_ecx ),
                            "d"( l_edx )
                          : "memory", "cc" );
    }
}

} // namespace system

// Constants
constexpr char g_commentSymbol = '#';

namespace color {

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

} // namespace color

// Concepts
template < typename... Arguments >
concept has_common_type = ( requires {
    typename std::common_type_t< Arguments... >;
} && !std::is_void_v< std::common_type_t< Arguments... > > );

template < typename T >
concept is_container = ( std::ranges::range< T > && requires( T _container ) {
    typename T::value_type;
    { _container.size() } -> std::convertible_to< std::size_t >;
} );

template < typename T >
concept is_struct = ( std::is_class_v< T > && !std::is_union_v< T > );

template < typename Lambda, typename ReturnType, typename... Arguments >
concept is_lambda =
    ( std::invocable< Lambda, Arguments... > &&
      std::convertible_to< std::invoke_result_t< Lambda, Arguments... >,
                           ReturnType > );

template < typename T >
concept is_formattable = ( requires( const T& _argument ) {
    { std::format( "{}", _argument ) } -> std::convertible_to< std::string >;
} );

// Utility macros ( no side-effects )
#define STRINGIFY( _value ) #_value

// Debug utility functions ( side-effects )

#if defined( assert )

#undef assert

#endif

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

// Utility functions ( no side-effects )
[[nodiscard]] constexpr auto makeU128( std::string_view _string ) -> uint128_t {
    if ( !std::ranges::all_of( _string, []( char _symbol ) -> bool {
             return ( ( _symbol >= '0' ) && ( _symbol <= '9' ) );
         } ) ) {
        // TODO: Write message
        stdfunc::trap();
    }

    uint128_t l_returnValue = 0;

    for ( const char _symbol : _string ) {
        l_returnValue = ( l_returnValue * 10 + ( _symbol - '0' ) );
    }

    return ( l_returnValue );
}

namespace literals {

[[nodiscard]] consteval auto operator""_b( char _symbol ) -> std::byte {
    return ( static_cast< std::byte >( _symbol ) );
}

[[nodiscard]] consteval auto operator""_b( unsigned long long _symbol )
    -> std::byte {
    assert( _symbol <= 0xFF );

    return ( static_cast< std::byte >( _symbol ) );
}

template < typename SymbolTypes, SymbolTypes... _symbols >
[[nodiscard]] consteval auto operator""_bytes() {
    assert( ( ... && ( _symbols <= 0xFF ) ) );

    return ( std::array< std::byte, sizeof...( _symbols ) >{
        std::byte{ _symbols }... } );
}

template < typename SymbolTypes, SymbolTypes... _symbols >
[[nodiscard]] consteval auto operator""_u128() -> uint128_t {
    assert( ( ... && ( _symbols <= 0xFF ) ) );

    return ( makeU128( { _symbols... } ) );
}

} // namespace literals

template < template < typename > typename Container, typename... Arguments >
[[nodiscard]] constexpr auto makeVariantContainer( Arguments&&... _arguments ) {
    using variant_t = std::variant< std::decay_t< Arguments >... >;

    return ( Container< variant_t >{
        variant_t( std::forward< Arguments >( _arguments ) )... } );
}

template < template < typename, size_t > typename Container,
           typename... Arguments >
[[nodiscard]] constexpr auto makeVariantContainer( Arguments&&... _arguments ) {
    using variant_t = std::variant< std::decay_t< Arguments >... >;

    return ( Container< variant_t, sizeof...( Arguments ) >{
        variant_t( std::forward< Arguments >( _arguments ) )... } );
}

template < std::integral T >
[[nodiscard]] constexpr auto bitsToBytes( T _bits ) -> T {
    return ( ( _bits + 7 ) / 8 );
}

template < std::integral T >
[[nodiscard]] constexpr auto lengthOfNumber( T _number ) -> size_t {
    return ( ( _number < 10ULL )                     ? ( 1 )
             : ( _number < 100ULL )                  ? ( 2 )
             : ( _number < 1000ULL )                 ? ( 3 )
             : ( _number < 10000ULL )                ? ( 4 )
             : ( _number < 100000ULL )               ? ( 5 )
             : ( _number < 1000000ULL )              ? ( 6 )
             : ( _number < 10000000ULL )             ? ( 7 )
             : ( _number < 100000000ULL )            ? ( 8 )
             : ( _number < 1000000000ULL )           ? ( 9 )
             : ( _number < 10000000000ULL )          ? ( 10 )
             : ( _number < 100000000000ULL )         ? ( 11 )
             : ( _number < 1000000000000ULL )        ? ( 12 )
             : ( _number < 10000000000000ULL )       ? ( 13 )
             : ( _number < 100000000000000ULL )      ? ( 14 )
             : ( _number < 1000000000000000ULL )     ? ( 15 )
             : ( _number < 10000000000000000ULL )    ? ( 16 )
             : ( _number < 100000000000000000ULL )   ? ( 17 )
             : ( _number < 1000000000000000000ULL )  ? ( 18 )
             : ( _number < 10000000000000000000ULL ) ? ( 19 )
                                                     : ( 20 ) );
}

[[nodiscard]] constexpr auto isSpace( char _symbol ) -> bool {
    return ( ( _symbol == ' ' ) || ( _symbol == '\f' ) || ( _symbol == '\n' ) ||
             ( _symbol == '\r' ) || ( _symbol == '\t' ) ||
             ( _symbol == '\v' ) );
}

[[nodiscard]] constexpr auto sanitizeString( std::string_view _string ) {
    return ( _string | std::views::drop_while( isSpace ) |
             std::views::take_while( []( char _symbol ) constexpr -> bool {
                 return ( _symbol != g_commentSymbol );
             } ) |
             std::views::reverse | std::views::drop_while( isSpace ) |
             std::views::reverse );
}

namespace hash {

// FNV-1A for 32bits, 64bits and 128bits
template < std::unsigned_integral T >
[[nodiscard]] constexpr auto weak( std::span< const std::byte > _data ) -> T {
    assert( _data.size() );

    T l_offsetBasis = 0;
    T l_prime = 0;

    if constexpr ( std::is_same_v< T, uint32_t > ) {
        l_offsetBasis = 0x811C9DC5;
        l_prime = 0x1000193;

    } else if constexpr ( std::is_same_v< T, uint64_t > ) {
        l_offsetBasis = 0xCBF29CE484222325;
        l_prime = 0x100000001b3;

    } else if constexpr ( std::is_same_v< T, uint128_t > ) {
        l_offsetBasis = makeU128( "0x6C62272E07BB014262B821756295C58D" );
        l_prime = makeU128( "0x1000000000000000000013b" );

    } else {
        // TODO: Message
        static_assert( false );
    }

    T l_hash = l_offsetBasis;

    for ( const uint8_t _item :
          _data | std::views::transform( []( std::byte _byte ) -> uint8_t {
              return ( static_cast< uint8_t >( _byte ) );
          } ) ) {
        l_hash ^= _item;

        l_hash *= l_prime;
    }

    return ( l_hash );
}

// xxHash3 for 64bits and 128bits
template < std::unsigned_integral T >
[[nodiscard]] constexpr auto balanced( std::span< const std::byte > _data,
                                       size_t _seed = 0x9E3779B1 ) -> T {
    if constexpr ( std::is_same_v< T, uint64_t > ) {
        return ( XXH3_64bits_withSeed( _data.data(), _data.size(), _seed ) );

    } else if constexpr ( std::is_same_v< T, uint128_t > ) {
        return ( XXH3_128bits_withSeed( _data.data(), _data.size(), _seed ) );

    } else {
        // TODO: Message
        static_assert( false );
    }
}

// TODO: Strong and Robust

} // namespace hash

// Utility functions ( side-effects )
namespace random {

// Seconds from midnight
extern const size_t g_compilationTimeAsSeed;

namespace number {

// Constexpr
// XOR-Shift for 32bits, 64bits and 128bits
template < std::unsigned_integral T >
[[nodiscard]] constexpr auto weak( T _seed ) -> T {
    T l_first = 0;
    T l_second = 0;
    T l_third = 0;

    if constexpr ( std::is_same_v< T, uint32_t > ) {
        l_first = 13;
        l_second = 17;
        l_third = 5;

    } else if constexpr ( std::is_same_v< T, uint64_t > ) {
        l_first = 13;
        l_second = 7;
        l_third = 17;

    } else if constexpr ( std::is_same_v< T, uint128_t > ) {
        l_first = 23;
        l_second = 17;
        l_third = 26;

    } else {
        // TODO: Message
        static_assert( false );
    }

    const auto l_data = std::bit_cast<
        std::array< std::byte, ( sizeof( T ) / sizeof( std::byte ) ) > >(
        _seed );

    _seed = hash::weak< T >( l_data );

    _seed ^= ( _seed << l_first );
    _seed ^= ( _seed >> l_second );
    _seed ^= ( _seed << l_third );

    return ( _seed );
}

// Runtime
using engine_t = std::
    conditional_t< ( sizeof( size_t ) > 4 ), std::mt19937_64, std::mt19937 >;

extern thread_local engine_t g_engine;

template < typename T >
    requires std::is_arithmetic_v< T >
auto strong( T _min, T _max ) -> T {
    using distribution_t =
        std::conditional_t< std::is_integral_v< T >,
                            std::uniform_int_distribution< T >,
                            std::uniform_real_distribution< T > >;

    return ( ( distribution_t( _min, _max ) )( g_engine ) );
}

template < typename T >
    requires std::is_arithmetic_v< T >
auto strong() -> T {
    using numericLimit_t = std::numeric_limits< T >;

    const auto l_max = numericLimit_t::max();

    if constexpr ( std::is_integral_v< T > ) {
        return ( ( std::uniform_int_distribution< T >( numericLimit_t::min(),
                                                       l_max ) )( g_engine ) );

    } else if constexpr ( std::is_floating_point_v< T > ) {
        return ( ( std::uniform_real_distribution< T >(
            numericLimit_t::lowest(), l_max ) )( g_engine ) );
    }
}

// TODO: Balanced and Robust

constexpr auto g_defaultNumberGenerator = weak< size_t >;

} // namespace number

template < typename Container >
    requires is_container< Container >
constexpr auto value(
    Container& _container,
    auto _randomNumberGenerator = number::g_defaultNumberGenerator ) ->
    typename Container::value_type& {
    assert( !_container.empty() );

    return ( _container.at(
        _randomNumberGenerator( 0, ( _container.size() - 1 ) ) ) );
}

template < typename Container >
    requires is_container< Container >
constexpr auto value(
    const Container& _container,
    auto _randomNumberGenerator = number::g_defaultNumberGenerator ) -> const
    typename Container::value_type& {
    assert( !_container.empty() );

    return ( _container.at(
        _randomNumberGenerator( 0, ( _container.size() - 1 ) ) ) );
}

template < typename Container >
    requires is_container< Container >
constexpr auto view( Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) |
             std::views::transform( [ & ]( auto ) constexpr -> auto {
                 return ( value( _container ) );
             } ) );
}

template < typename Container >
    requires is_container< Container >
constexpr auto view( const Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) |
             std::views::transform( [ & ]( auto ) constexpr -> auto {
                 return ( value( _container ) );
             } ) );
}

template < typename Container, typename T = typename Container::value_type >
    requires( is_container< Container > && std::is_arithmetic_v< T > )
constexpr void fill(
    Container& _container,
    T _min,
    T _max,
    auto _randomNumberGenerator = number::g_defaultNumberGenerator ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( _randomNumberGenerator( _min, _max ) );
    } );
}

template < typename Container, typename T = typename Container::value_type >
    requires( is_container< Container > && std::is_same_v< T, std::byte > )
constexpr void fill(
    Container& _container,
    uint8_t _min,
    uint8_t _max,
    auto _randomNumberGenerator = number::g_defaultNumberGenerator ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return (
            static_cast< std::byte >( _randomNumberGenerator( _min, _max ) ) );
    } );
}

template < typename Container, typename T = typename Container::value_type >
    requires( is_container< Container > && std::is_arithmetic_v< T > )
constexpr void fill(
    Container& _container,
    auto _randomNumberGenerator = number::g_defaultNumberGenerator ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( _randomNumberGenerator() );
    } );
}

template < typename Container, typename T = typename Container::value_type >
    requires( is_container< Container > && std::is_same_v< T, std::byte > )
constexpr void fill(
    Container& _container,
    auto _randomNumberGenerator = number::g_defaultNumberGenerator ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( static_cast< std::byte >( _randomNumberGenerator() ) );
    } );
}

} // namespace random

namespace filesystem {

// Utility OS specific functions ( no side-effects )
[[nodiscard]] auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::filesystem::path >;

namespace {

template < typename Matcher >
    requires is_lambda< Matcher, bool, const std::string& >
[[nodiscard]] auto _getPathsByRegexp( std::string_view _directory,
                                      Matcher _matcher )
    -> std::vector< std::filesystem::path > {
    std::vector< std::filesystem::path > l_returnValue;

    for ( const auto& _entry :
          std::filesystem::directory_iterator( _directory ) ) {
        if ( !_entry.is_regular_file() ) {
            continue;
        }

        const std::string l_filename = _entry.path().filename().string();

        if ( _matcher( l_filename ) ) {
            l_returnValue.emplace_back( _entry.path() );
        }
    }

    return ( l_returnValue );
}

} // namespace

// Runtime regexp
[[nodiscard]] inline auto getPathsByRegexp( std::string& _regexp,
                                            std::string_view _directory )
    -> std::vector< std::filesystem::path > {
    std::regex l_matcher( _regexp );

    return ( _getPathsByRegexp(
        _directory, [ & ]( const std::string& _fileName ) -> bool {
            return ( std::regex_match( _fileName, l_matcher ) );
        } ) );
}

// Compile-time regexp
template < size_t N >
    requires( N > 0 )
[[nodiscard]] auto getPathsByRegexp( const ctll::fixed_string< N >& _regexp,
                                     std::string_view _directory )
    -> std::vector< std::filesystem::path > {
    constexpr auto l_matcher = ctre::match< _regexp >;

    return ( _getPathsByRegexp( _directory,
                                [ & ]( const std::string& _fileName ) -> auto {
                                    return ( l_matcher( _fileName ) );
                                } ) );
}

} // namespace filesystem

namespace compress {

/**
 * @brief Compress a UTF-8 (or arbitrary) text string.
 *
 * This is a convenience wrapper around a fast, stream-oriented text compressor
 * (current implementation: **Snappy**). The function returns a `std::string`
 * containing the compressed frame. The compressed output is binary data and
 * may contain NUL bytes — treat it as opaque.
 *
 * @param _text  Input text to compress. Treated as a sequence of bytes; no
 *               string encoding validation is performed.
 * @param _level Compression level: Level 1 is the fastest Level 2 is a little
 *               slower but provides better compression.
 *
 * @return `std::optional<std::string>`:
 *         - contains compressed bytes on success;
 *         - `std::nullopt` on failure (e.g. underlying library error,
 *           out-of-memory, or other API-level failure).
 *
 * @threadsafe Safe to call concurrently from multiple threads as long as the
 *            underlying compressor library is initialized appropriately and
 *            you do not mutate any shared global compressor state. The
 *            implementation should allocate thread-local temporaries.
 *
 * @complexity Time: roughly linear in `_text.size()`; exact constant factors
 *             depend on compressor. Memory: allocates an output buffer sized
 *             to the compressor's worst-case output (implementation dependent).
 *
 * @example
 * std::optional<std::string> c = compress::text("hello world");
 * if (c) {
 *     // c->data() contains compressed bytes (not a printable string)
 * }
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _text, size_t _level = 1 )
    -> std::optional< std::string >;

/**
 * @brief Compress arbitrary binary data.
 *
 * This function compresses a block of bytes using a general-purpose binary
 * compressor (current implementation: **Zstandard (zstd)**). It returns a
 * vector of bytes containing the compressed frame. The result should be
 * treated as opaque binary data.
 *
 * @param _data  Input data to compress (view into caller-owned memory).
 * @param _level Compression level. For zstd typical valid values are
 *               1..22 (implementation-dependent). Lower values favor speed,
 *               higher values favor compression ratio. Default is `3`
 *               (a reasonable speed/ratio tradeoff).
 *
 *               Rough guideline (zstd):
 *                 - 1 : fastest, lowest compression
 *                 - 3 : fast, good default
 *                 - 9 : slower, noticeably better compression
 *                 - 19-22 : slowest, best compression (may be very slow/memory
 * heavy)
 *
 * @return `std::optional<std::vector<std::byte>>`:
 *         - contains compressed bytes on success;
 *         - `std::nullopt` on failure (invalid arguments, compression error,
 *           or underlying library failure).
 *
 * @threadsafe Thread-safe to call concurrently unless the implementation uses
 *            a shared mutable compressor context (it typically won't). If you
 *            plan high-concurrency workloads, benchmark and consider per-thread
 *            contexts or streaming APIs.
 *
 * @complexity Time: roughly linear in `_data.size()` with constants depending
 *             on `_level`. Memory: allocates output buffer of size proportional
 *             to compressor worst-case.
 *
 * @example
 * std::vector<std::byte> in = ...;
 * auto outOpt = compress::data(std::span(in), level=5);
 * if (outOpt) { write outOpt->data() to disk/network  }
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< const std::byte > _data, size_t _level = 3 )
    -> std::optional< std::vector< std::byte > >;

} // namespace compress

namespace decompress {

/**
 * @brief Decompress a text frame produced by `compress::text`.
 *
 * Attempts to decompress the binary frame in `_data` and return the original
 * text as `std::string`. Returns `std::nullopt` on failure (invalid frame,
 * corruption, unsupported format, or other decompression error).
 *
 * @param _data Compressed frame (binary). The function treats the bytes as
 *              opaque input for the decompressor.
 *
 * @return `std::optional<std::string>`:
 *         - decompressed original string on success;
 *         - `std::nullopt` on failure.
 *
 * @threadsafe Safe to call concurrently from multiple threads provided the
 *            underlying library does not require exclusive initialization.
 *
 * @complexity Time: roughly linear in the size of the decompressed data.
 *
 * @note If `compress::text` is implemented with Snappy (no framing for original
 *       size), the decompressor will use the framing/format produced by that
 *       compressor. If your compressed frames do not include the original
 *       size, the decompressor must rely on the compressor frame metadata.
 *
 * @example
 * auto de = decompress::text(compressed_string);
 * if (de) { std::string s = std::move(*de); }
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _data )
    -> std::optional< std::string >;

/**
 * @brief Decompress binary data produced by `compress::data`.
 *
 * Decompress a binary compressed frame into a `std::vector<std::byte>` of the
 * original size `_originalSize`. Some compressors (like zstd) may store the
 * original size in the frame; however many APIs expect the caller to provide
 * the expected decompressed size. If your compressor stores the original size
 * in the frame, `_originalSize` can be unused — consult your implementation.
 *
 * @param _data         Compressed frame bytes.
 * @param _originalSize Expected size of the decompressed output in bytes.
 *                      The function may use this to allocate the output buffer
 *                      and to validate the decompressed result. Passing an
 *                      incorrect `_originalSize` may cause decompression to
 *                      fail or return truncated/incorrect data.
 *
 * @return `std::optional<std::vector<std::byte>>`:
 *         - decompressed byte vector on success;
 *         - `std::nullopt` on failure (bad frame, corruption, mismatch with
 *           `_originalSize`, etc.).
 *
 * @threadsafe Safe to call concurrently, subject to underlying library rules.
 *
 * @complexity Time: approximately linear in `_originalSize` (or the
 *             decompressed amount).
 *
 * @example
 * auto outOpt = decompress::data(std::span(compressedBytes),
 * expected_original_size); if (outOpt) { write(outOpt->data(), outOpt->size());
 * }
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< const std::byte > _data,
                         size_t _originalSize )
    -> std::optional< std::vector< std::byte > >;

} // namespace decompress

namespace meta {

template < typename T >
concept is_reflectable = glz::has_reflect< T >;

/**
 * @brief Convert type into struct with metadata
 *
 * If T is struct or class, then fields are
 * struct reflect< T > {
 *       static constexpr auto size = 0;
 *       static constexpr auto values = tuple{};
 *       static constexpr std::array<sv, 0> keys{};
 *
 *       template <size_t I>
 *       using type = std::nullptr_t;
 * }
 */
template < typename T >
using reflect_t = glz::reflect< T >;

// Functions that take instance
template < typename T, typename Callback >
    requires is_reflectable< T >
constexpr void iterateStructTopMostFields( T&& _instance,
                                           Callback&& _callback ) {
    glz::for_each_field( std::forward< T >( _instance ),
                         std::forward< Callback >( _callback ) );
}

// Functions that take type
template < typename T, typename Callback >
    requires is_reflectable< T >
constexpr void iterateStructTopMostFields( Callback&& _callback ) {
    constexpr T l_instance{};

    glz::for_each_field( l_instance, std::forward< Callback >( _callback ) );
}

template < typename T >
    requires is_reflectable< T >
consteval auto hasMemberWithName( std::string_view _name ) -> bool {
    return ( std::ranges::contains( reflect_t< T >::keys, _name ) );
}

} // namespace meta

} // namespace stdfunc
