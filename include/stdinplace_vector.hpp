#pragma once

#include <glaze/containers/inplace_vector.hpp>

namespace stdfunc {

// TODO: Add N > 0
template < typename T, size_t N >
using inplaceVector_t = glz::inplace_vector< T, N >;

} // namespace stdfunc
