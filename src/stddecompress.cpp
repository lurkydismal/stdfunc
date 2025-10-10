#include "stddecompress.hpp"

#include <snappy.h>
#include <zstd.h>

namespace stdfunc::decompress {

auto text( std::string_view _data ) -> std::optional< std::string > {
    std::optional< std::string > l_returnValue = std::nullopt;

    do {
        if ( _data.empty() ) [[unlikely]] {
            break;
        }

        std::string l_decompressed;

        if ( !snappy::Uncompress( _data.data(), _data.size(),
                                  &l_decompressed ) ) [[unlikely]] {
            break;
        }

        l_returnValue = l_decompressed;
    } while ( false );

    return ( l_returnValue );
}

auto data( std::span< const std::byte > _data, size_t _originalSize )
    -> std::optional< std::vector< std::byte > > {
    std::optional< std::vector< std::byte > > l_returnValue = std::nullopt;

    do {
        if ( _data.empty() ) [[unlikely]] {
            break;
        }

        if ( !_originalSize ) [[unlikely]] {
            break;
        }

        std::vector< std::byte > l_decompressed( _originalSize );

        const size_t l_decompressedSize =
            ZSTD_decompress( l_decompressed.data(), l_decompressed.size(),
                             _data.data(), _data.size() );

        if ( ZSTD_isError( l_decompressedSize ) ) [[unlikely]] {
            break;
        }

        l_decompressed.resize( l_decompressedSize );

        l_returnValue = l_decompressed;
    } while ( false );

    return ( l_returnValue );
}

} // namespace stdfunc::decompress
