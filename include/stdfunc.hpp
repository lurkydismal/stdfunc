#pragma once

#include <concepts>
#include <cstddef>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

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

namespace stdfunc {

// Constants
constexpr char g_commentSymbol = '#';

// Utility macros ( no side-effects )
#define STRINGIFY( _value ) #_value

// Utility functions ( no side-effects )
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

} // namespace stdfunc
