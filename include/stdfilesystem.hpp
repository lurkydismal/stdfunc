#pragma once

#include <filesystem>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#if __has_include( "ctre/wrapper.hpp" )

#define HAS_CONSTEXPR_REGEXP

#include "ctre/wrapper.hpp"

#endif

#include "stdconcepts.hpp"

namespace stdfunc::filesystem {

// Utility OS specific functions ( no side-effects )
[[nodiscard]] auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::filesystem::path >;

namespace {

template < typename Matcher >
    requires is_lambda< Matcher, bool, const std::string& >
[[nodiscard]] auto _getPathsByRegexp( std::string_view _directory,
                                      Matcher _matcher )
    -> std::vector< std::filesystem::path > {
    std::vector< std::filesystem::path > l_returnValue;

    for ( const auto& _entry :
          std::filesystem::directory_iterator( _directory ) ) {
        if ( !_entry.is_regular_file() ) {
            continue;
        }

        const std::string l_filename = _entry.path().filename().string();

        if ( _matcher( l_filename ) ) {
            l_returnValue.emplace_back( _entry.path() );
        }
    }

    return ( l_returnValue );
}

} // namespace

// Runtime regexp
[[nodiscard]] inline auto getPathsByRegexp( std::string& _regexp,
                                            std::string_view _directory )
    -> std::vector< std::filesystem::path > {
    std::regex l_matcher( _regexp );

    return ( _getPathsByRegexp(
        _directory, [ & ]( const std::string& _fileName ) -> bool {
            return ( std::regex_match( _fileName, l_matcher ) );
        } ) );
}

#if defined( HAS_CONSTEXPR_REGEXP )

// Compile-time regexp
template < size_t N >
    requires( N > 0 )
[[nodiscard]] auto getPathsByRegexp( const ctll::fixed_string< N >& _regexp,
                                     std::string_view _directory )
    -> std::vector< std::filesystem::path > {
    constexpr auto l_matcher = ctre::match< _regexp >;

    return ( _getPathsByRegexp( _directory,
                                [ & ]( const std::string& _fileName ) -> auto {
                                    return ( l_matcher( _fileName ) );
                                } ) );
}

#endif

} // namespace stdfunc::filesystem
