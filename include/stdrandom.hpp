#pragma once

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>

#include "std128.hpp"
#include "stdhash.hpp"

// Utility functions ( side-effects )
namespace stdfunc::random {

#if 0
// TODO: Improve
// Seconds from midnight
extern const size_t g_compilationTimeAsSeed;
#endif

// Golden ratio
template < std::integral T, typename ReturnT = std::make_unsigned_t< T > >
constexpr T g_goldenRatioSeed = [] consteval -> ReturnT {
    if constexpr ( sizeof( T ) == sizeof( uint32_t ) ) {
        return ( 0x9E3779B9U );

    } else if constexpr ( sizeof( T ) == sizeof( uint64_t ) ) {
        return ( 0x9E3779B97F4A7C15UL );

    } else if constexpr ( sizeof( T ) == sizeof( uint128_t ) ) {
        return ( makeU128( "9E3779B97F4A7C15F39CC0605CEDC835" ) );
    }
}();

// TODO: Weak: add float variant
namespace number {

// Constexpr
// XOR-Shift*( multiply ) for 32bits, 64bits and 128bits
template < std::integral T, typename ReturnT = std::make_unsigned_t< T > >
[[nodiscard]] constexpr auto weak( T _seed = g_goldenRatioSeed< T > )
    -> ReturnT {
    ReturnT l_first = 0;
    ReturnT l_second = 0;
    ReturnT l_third = 0;
    ReturnT l_multiplier = 0;

    if constexpr ( sizeof( T ) == sizeof( uint32_t ) ) {
        l_first = 7;
        l_second = 1;
        l_third = 9;
        l_multiplier = 0x9E3779B1;

    } else if constexpr ( sizeof( T ) == sizeof( uint64_t ) ) {
        l_first = 12;
        l_second = 25;
        l_third = 27;
        l_multiplier = 0x2545F4914F6CDD1D;

    } else if constexpr ( sizeof( T ) == sizeof( uint128_t ) ) {
        l_first = 35;
        l_second = 21;
        l_third = 45;
        l_multiplier = 0x2545F4914F6CDD1D;

    } else {
        // TODO: Message
        static_assert( false );
    }

    static T l_seed = 0;

    if ( !l_seed ) [[unlikely]] {
        const auto l_data = std::bit_cast<
            std::array< std::byte, ( sizeof( T ) / sizeof( std::byte ) ) > >(
            _seed );

        l_seed = hash::weak< ReturnT >( l_data );
    }

    l_seed ^= ( l_seed << l_first );
    l_seed ^= ( l_seed >> l_second );
    l_seed ^= ( l_seed << l_third );

    return ( l_seed * l_multiplier );
}

template < std::integral T >
[[nodiscard]] constexpr auto weak( T _min,
                                   T _max,
                                   T _seed = g_goldenRatioSeed< T > ) -> T {
    const T l_range = ( _max - _min + 1 );
    const T l_limit =
        ( ( std::numeric_limits< T >::max() / l_range ) * l_range );

    std::make_unsigned_t< T > l_result = 0;

    do {
        l_result = weak< T >( _seed );
    } while ( l_result >= l_limit );

    return ( _min + ( l_result % l_range ) );
}

// Runtime
using engine_t = std::
    conditional_t< ( sizeof( size_t ) > 4 ), std::mt19937_64, std::mt19937 >;

extern thread_local engine_t g_engine;

template < typename T >
    requires std::is_arithmetic_v< T >
auto balanced( T _min, T _max ) -> T {
    using distribution_t =
        std::conditional_t< std::is_integral_v< T >,
                            std::uniform_int_distribution< T >,
                            std::uniform_real_distribution< T > >;

    return ( ( distribution_t( _min, _max ) )( g_engine ) );
}

template < typename T >
    requires std::is_arithmetic_v< T >
auto balanced() -> T {
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

// TODO: Strong and Robust

template < typename T >
    requires std::is_arithmetic_v< T >
constexpr auto g_defaultNumberGenerator = []( auto... _arguments ) -> T {
    return ( balanced< T >( _arguments... ) );
};

} // namespace number

template < is_container Container, typename T = typename Container::value_type >
constexpr auto value( Container& _container ) -> T& {
    assert( !_container.empty() );

    return ( _container.at( number::g_defaultNumberGenerator< T >(
        0, ( _container.size() - 1 ) ) ) );
}

template < is_container Container, typename T = typename Container::value_type >
constexpr auto value( const Container& _container ) -> const T& {
    assert( !_container.empty() );

    return ( _container.at( number::g_defaultNumberGenerator< T >(
        0, ( _container.size() - 1 ) ) ) );
}

template < is_container Container >
constexpr auto view( Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) |
             std::views::transform( [ & ]( auto ) constexpr -> auto {
                 return ( value( _container ) );
             } ) );
}

template < is_container Container >
constexpr auto view( const Container& _container ) {
    assert( !_container.empty() );

    return ( std::views::iota( 0 ) |
             std::views::transform( [ & ]( auto ) constexpr -> auto {
                 return ( value( _container ) );
             } ) );
}

template < is_container Container, typename T = typename Container::value_type >
    requires std::is_arithmetic_v< T >
constexpr void fill( Container& _container, T _min, T _max ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( number::g_defaultNumberGenerator< T >( _min, _max ) );
    } );
}

template < is_container Container, typename T = typename Container::value_type >
    requires std::is_same_v< T, std::byte >
constexpr void fill( Container& _container, uint8_t _min, uint8_t _max ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( static_cast< std::byte >(
            number::g_defaultNumberGenerator< uint32_t >( _min, _max ) ) );
    } );
}

template < is_container Container, typename T = typename Container::value_type >
    requires std::is_arithmetic_v< T >
constexpr void fill( Container& _container ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( number::g_defaultNumberGenerator< T >() );
    } );
}

template < is_container Container, typename T = typename Container::value_type >
    requires std::is_same_v< T, std::byte >
constexpr void fill( Container& _container ) {
    std::ranges::generate( _container, [ & ] constexpr -> auto {
        return ( static_cast< std::byte >(
            number::g_defaultNumberGenerator< uint32_t >() ) );
    } );
}

} // namespace stdfunc::random
