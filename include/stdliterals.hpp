#pragma once

#include <array>
#include <cstddef>

#include "std128.hpp"
#include "stddebug.hpp"

namespace stdfunc::literals {

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

// TODO: Accept hex
template < typename SymbolTypes, SymbolTypes... _symbols >
[[nodiscard]] consteval auto operator""_u128() -> uint128_t {
    assert( ( ... && ( _symbols <= 0xFF ) ) );

    return ( makeU128( { _symbols... } ) );
}

} // namespace stdfunc::literals
