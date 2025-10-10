#pragma once

#include <concepts>
#include <format>
#include <type_traits>

namespace stdfunc {

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

} // namespace stdfunc
