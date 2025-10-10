#pragma once

#include <algorithm>
#include <string_view>

#include "stddebug.hpp"

using uint128_t = __uint128_t;

namespace stdfunc {

// TODO: Accept hex
[[nodiscard]] constexpr auto makeU128( std::string_view _string ) -> uint128_t {
    if ( !std::ranges::all_of( _string, []( char _symbol ) -> bool {
             return ( ( _symbol >= '0' ) && ( _symbol <= '9' ) );
         } ) ) {
        // TODO: Write message
        trap();
    }

    uint128_t l_returnValue = 0;

    for ( const char _symbol : _string ) {
        l_returnValue = ( l_returnValue * 10 + ( _symbol - '0' ) );
    }

    return ( l_returnValue );
}

} // namespace stdfunc
