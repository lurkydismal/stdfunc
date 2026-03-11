#pragma once

#include <type_traits>

namespace stdfunc {

template < class T = void >
void todo() {
    static_assert( !std::is_same_v< T, T >, "TODO: not implemented" );
}

} // namespace stdfunc
