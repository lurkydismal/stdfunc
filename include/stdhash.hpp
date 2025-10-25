#pragma once

#include <xxh3.h>

#include <concepts>
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>

#include "rapidhash.h"
#include "std128.hpp"

namespace stdfunc::hash {

// FNV-1A for 32bits, 64bits and 128bits
template < std::integral T, typename ReturnT = std::make_unsigned_t< T > >
[[nodiscard]] constexpr auto weak( std::span< const std::byte > _data )
    -> ReturnT {
    assert( _data.size() );

    ReturnT l_offsetBasis = 0;
    ReturnT l_prime = 0;

    if constexpr ( sizeof( T ) == sizeof( uint32_t ) ) {
        l_offsetBasis = 0x811C9DC5;
        l_prime = 0x1000193;

    } else if constexpr ( sizeof( T ) == sizeof( uint64_t ) ) {
        l_offsetBasis = 0xCBF29CE484222325;
        l_prime = 0x100000001b3;

    } else if constexpr ( sizeof( T ) == sizeof( uint128_t ) ) {
        l_offsetBasis = makeU128( "6C62272E07BB014262B821756295C58D" );
        l_prime = makeU128( "1000000000000000000013b" );

    } else {
        // TODO: Message
        static_assert( false );
    }

    ReturnT l_hash = l_offsetBasis;

    for ( const uint8_t _item :
          _data | std::views::transform( []( std::byte _byte ) -> uint8_t {
              return ( static_cast< uint8_t >( _byte ) );
          } ) ) {
        l_hash ^= _item;

        l_hash *= l_prime;
    }

    return ( l_hash );
}

// rapidhash for 64bits and xxHash3 for 128bits
// TODO: Make constexpr
template < std::integral T, typename ReturnT = std::make_unsigned_t< T > >
[[nodiscard]] constexpr auto balanced( std::span< const std::byte > _data,
                                       size_t _seed = 0x9E3779B1 ) -> ReturnT {
    assert( _data.size() );

    if constexpr ( sizeof( T ) == sizeof( uint64_t ) ) {
        return ( rapidhash_withSeed( _data.data(), _data.size(), _seed ) );

    } else if constexpr ( sizeof( T ) == sizeof( uint128_t ) ) {
        XXH128_hash_t l_temp =
            XXH3_128bits_withSeed( _data.data(), _data.size(), _seed );

        return ( ( static_cast< uint128_t >( l_temp.high64 ) << 64 ) |
                 l_temp.low64 );

    } else {
        // TODO: Message
        static_assert( false );
    }
}

// TODO: Strong and Robust

} // namespace stdfunc::hash
