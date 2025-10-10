#pragma once

#include <glaze/core/reflect.hpp>

#include <algorithm>
#include <utility>

namespace stdfunc::meta {

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
template < is_reflectable T, typename Callback >
constexpr void iterateStructTopMostFields( T&& _instance,
                                           Callback&& _callback ) {
    glz::for_each_field( std::forward< T >( _instance ),
                         std::forward< Callback >( _callback ) );
}

// Functions that take type
template < is_reflectable T, typename Callback >
constexpr void iterateStructTopMostFields( Callback&& _callback ) {
    T l_instance{};

    glz::for_each_field( l_instance, std::forward< Callback >( _callback ) );
}

template < is_reflectable T >
consteval auto hasMemberWithName( std::string_view _name ) -> bool {
    return ( std::ranges::contains( reflect_t< T >::keys, _name ) );
}

} // namespace stdfunc::meta
