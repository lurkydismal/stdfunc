#include "stdfunc.hpp"

#include <snappy.h>
#include <zstd.h>

namespace stdfunc {

// Seconds from midnight
const size_t random::g_compilationTimeAsSeed =
    ( ( ( ( ( __TIME__[ 0 ] - '0' ) * 10 ) + ( __TIME__[ 1 ] - '0' ) ) *
        3600 ) +
      ( ( ( ( __TIME__[ 3 ] - '0' ) * 10 ) + ( __TIME__[ 4 ] - '0' ) ) * 60 ) +
      ( ( ( __TIME__[ 6 ] - '0' ) * 10 ) + ( __TIME__[ 7 ] - '0' ) ) );

thread_local random::number::engine_t g_engine{
#if defined( STDFUNC_RANDOM_CONSTEXPR )

    // Golden ratio
    0x9E3779B97F4A7C15

#else

    std::random_device{}()

#endif
};

namespace compress {

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

} // namespace compress

namespace decompress {

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

} // namespace decompress

namespace filesystem {

auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::filesystem::path > {
    std::optional< std::filesystem::path > l_returnValue = std::nullopt;

    do {
        std::array< char, PATH_MAX > l_executablePath{};

        // Get executable path
        {
            const ssize_t l_executablePathLength = readlink(
                "/proc/self/exe", l_executablePath.data(), ( PATH_MAX - 1 ) );

            if ( l_executablePathLength == -1 ) [[unlikely]] {
                break;
            }
        }

        l_returnValue =
            std::filesystem::path( std::string_view( l_executablePath ) )
                .remove_filename();
    } while ( false );

    return ( l_returnValue );
}

} // namespace filesystem

} // namespace stdfunc
