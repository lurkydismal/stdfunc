#pragma once

#include <glaze/containers/flat_map.hpp>

#include "stdinplace_vector.hpp"

namespace stdfunc {

namespace {

template < typename K, typename V >
using item_t = std::pair< K, V >;

}

template < typename K,
           typename V,
           typename Comparator = std::less<>,
           typename Container = std::vector< item_t< K, V > > >
using flatMap_t = glz::flat_map< K, V, Comparator, Container >;

// TODO: Make helper to deduce size
template < typename K,
           typename V,
           size_t N,
           typename Comparator = std::less<>,
           typename Container = stdfunc::inplaceVector_t< item_t< K, V >, N > >
using inplaceFlatMap_t = glz::flat_map< K, V, Comparator, Container >;

} // namespace stdfunc
