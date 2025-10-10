#include "stdcompress.hpp"

#include <snappy.h>
#include <zstd.h>

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace stdfunc::compress {

auto text( std::string_view _text, size_t _level )
    -> std::optional< std::string > {
    std::optional< std::string > l_returnValue = std::nullopt;

    do {
        if ( _text.empty() ) [[unlikely]] {
            break;
        }

        if ( !_level ) [[unlikely]] {
            break;
        }

        std::string l_compressed;

        if ( !snappy::Compress( _text.data(), _text.size(), &l_compressed,
                                snappy::CompressionOptions( _level ) ) )
            [[unlikely]] {
            break;
        }

        l_returnValue = l_compressed;
    } while ( false );

    return ( l_returnValue );
}

auto data( std::span< const std::byte > _data, size_t _level )
    -> std::optional< std::vector< std::byte > > {
    std::optional< std::vector< std::byte > > l_returnValue = std::nullopt;

    do {
        if ( _data.empty() ) [[unlikely]] {
            break;
        }

        if ( !_level ) [[unlikely]] {
            break;
        }

        const size_t l_maxCompressedSize = ZSTD_compressBound( _data.size() );

        std::vector< std::byte > l_compressed( l_maxCompressedSize );

        const size_t l_compressedSize =
            ZSTD_compress( l_compressed.data(), l_compressed.size(),
                           _data.data(), _data.size(), _level );

        if ( ZSTD_isError( l_compressedSize ) ) [[unlikely]] {
            break;
        }

        l_compressed.resize( l_compressedSize );

        l_returnValue = l_compressed;
    } while ( false );

    return ( l_returnValue );
}

} // namespace stdfunc::compress
