#include "stdfunc.hpp"

#include <glaze/glaze.hpp>

#include <algorithm>
#include <numeric>
#include <unordered_set>

#include "test.hpp"

using namespace stdfunc;
using namespace stdfunc::literals;

TEST( stdfunc, STRINGIFY ) {
    EXPECT_EQ( STRINGIFY( Tessst ), "Tessst" );
    EXPECT_EQ( STRINGIFY( stdfunc::color::g_green ),
               "stdfunc::color::g_green" );
}

TEST( stdfunc, _b ) {
    {
        constexpr std::byte l_constexprCheck = 0_b;

        static_assert( l_constexprCheck == std::byte{} );
    }

    {
        constexpr std::byte l_constexprCheck = '0'_b;

        static_assert( l_constexprCheck == std::byte{ 0x30 } );
    }

    EXPECT_EQ( 'A'_b, std::byte{ 0x41 } );
    EXPECT_EQ( 'a'_b, std::byte{ 0x61 } );
    EXPECT_EQ( 'G'_b, std::byte{ 0x47 } );

    EXPECT_EQ( 0_b, std::byte{ 0 } );
    EXPECT_EQ( 10_b, std::byte{ 0xA } );
    EXPECT_EQ( 17_b, std::byte{ 0x11 } );
}

TEST( stdfunc, _bytes ) {
    constexpr auto l_constexprCheck = "C"_bytes;

    static_assert( l_constexprCheck == std::array{ std::byte{ 0x43 } } );

    std::array l_array = { std::byte{ 0x46 }, std::byte{ 0x46 },
                           std::byte{ 0x46 } };

    EXPECT_EQ( "FFF"_bytes, l_array );
}

TEST( stdfunc, bitsToBytes ) {
    EXPECT_EQ( bitsToBytes( 0 ), 0 );

    EXPECT_EQ( bitsToBytes( 8 ), 1 );
    EXPECT_EQ( bitsToBytes( 16 ), 2 );
    EXPECT_EQ( bitsToBytes( 32 ), 4 );

    EXPECT_EQ( bitsToBytes( 1 ), 1 );
    EXPECT_EQ( bitsToBytes( 7 ), 1 );
    EXPECT_EQ( bitsToBytes( 9 ), 2 );
    EXPECT_EQ( bitsToBytes( 15 ), 2 );
    EXPECT_EQ( bitsToBytes( 17 ), 3 );

    // 63 bits = 7.875 bytes -> 8
    EXPECT_EQ( bitsToBytes( 63 ), 8 );
    EXPECT_EQ( bitsToBytes( 64 ), 8 );
    EXPECT_EQ( bitsToBytes( 65 ), 9 );

    // Test different arithmetic types
    EXPECT_EQ( bitsToBytes< int >( 7 ), 1 );
    EXPECT_EQ( bitsToBytes< long long >( 9 ), 2 );
    EXPECT_EQ( bitsToBytes< unsigned >( 32 ), 4 );
    EXPECT_EQ( bitsToBytes< uint64_t >( 65 ), 9 );

    // Constexpr evaluation
    constexpr auto l_constexprCheck =
        ( bitsToBytes( 17 ) == 3 && bitsToBytes( 64 ) == 8 &&
          bitsToBytes( 0 ) == 0 );

    static_assert( l_constexprCheck );

    EXPECT_TRUE( l_constexprCheck );
}

TEST( stdfunc, lengthOfNumber ) {
    constexpr size_t l_constexprCheck = lengthOfNumber( size_t{ 123 } );

    static_assert( l_constexprCheck == 3 );

    size_t l_actualLengthFailed = 0;
    size_t l_expectedLengthFailed = 0;

    for ( auto _index : std::views::iota( size_t{}, 10'000'000uz ) ) {
        const size_t l_actualLength = lengthOfNumber( _index );
        const size_t l_expectedLength =
            ( ( _index == 0 ) ? ( 1 ) : ( log10( _index ) + 1 ) );

        if ( l_actualLength != l_expectedLength ) {
            l_actualLengthFailed = l_actualLength;
            l_expectedLengthFailed = l_expectedLength;
        }
    }

    EXPECT_EQ( l_actualLengthFailed, l_expectedLengthFailed );
}

TEST( stdfunc, isSpace ) {
    // True cases (standard C whitespace chars)
    EXPECT_TRUE( isSpace( ' ' ) );
    EXPECT_TRUE( isSpace( '\f' ) );
    EXPECT_TRUE( isSpace( '\n' ) );
    EXPECT_TRUE( isSpace( '\r' ) );
    EXPECT_TRUE( isSpace( '\t' ) );
    EXPECT_TRUE( isSpace( '\v' ) );

    // False cases (non-whitespace chars)
    EXPECT_FALSE( isSpace( 'a' ) );
    EXPECT_FALSE( isSpace( 'Z' ) );
    EXPECT_FALSE( isSpace( '0' ) );
    EXPECT_FALSE( isSpace( '_' ) );
    EXPECT_FALSE( isSpace( '-' ) );
    EXPECT_FALSE( isSpace( '\0' ) );

    // Constexpr check
    constexpr bool l_constexprCheck = ( isSpace( ' ' ) && isSpace( '\t' ) &&
                                        !isSpace( 'X' ) && !isSpace( '1' ) );

    static_assert( l_constexprCheck );

    EXPECT_TRUE( l_constexprCheck );
}

TEST( stdfunc, sanitizeString ) {
    // Normal case with spaces and a comment
    EXPECT_EQ( sanitizeString( "  Hello  World   # Comment here" ) |
                   std::ranges::to< std::string >(),
               "Hello  World" );

    // Leading and trailing spaces
    EXPECT_EQ(
        sanitizeString( "   test   " ) | std::ranges::to< std::string >(),
        "test" );

    // No spaces, no comments
    EXPECT_EQ( sanitizeString( "NoSpacesOrComments" ) |
                   std::ranges::to< std::string >(),
               "NoSpacesOrComments" );

    // Only spaces
    EXPECT_EQ( sanitizeString( "       " ) | std::ranges::to< std::string >(),
               "" );

    // Only comment
    EXPECT_EQ( sanitizeString( "   # This is a comment" ) |
                   std::ranges::to< std::string >(),
               "" );

    // Empty string
    EXPECT_EQ( sanitizeString( "" ) | std::ranges::to< std::string >(), "" );

    // Comment in the middle
    EXPECT_EQ( sanitizeString( "KeepThis # RemoveThis" ) |
                   std::ranges::to< std::string >(),
               "KeepThis" );
}

TEST( stdfunc, random$number$weak ) {
    {
        // Integral range
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            int l_value = random::number::weak< uint >( 1, 10 );
            EXPECT_GE( l_value, 1 );
            EXPECT_LE( l_value, 10 );
        }

        // Floating-point range
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            double l_value = random::number::weak< uint >( 0.5, 2.5 );
            EXPECT_GE( l_value, 0.5 );
            EXPECT_LE( l_value, 2.5 );
        }

#if 0
        // Different types
        auto l_intVal = random::number::weak< uint >( 5, 5 ); // degenerate range
        auto l_floatVal =
            random::number::weak< float >( 1.0f, 1.0f ); // degenerate range
        EXPECT_EQ( l_intVal, 5 );
        EXPECT_FLOAT_EQ( l_floatVal, 1.0f );
#endif

        // Variability check (not all results should be identical)
        bool l_sawDifferent = false;
        int l_first = random::number::weak< uint >( 1, 3 );
        for ( int l_i = 0; l_i < 50; ++l_i ) {
            if ( random::number::weak< uint >( 1, 3 ) != l_first ) {
                l_sawDifferent = true;
                break;
            }
        }

        EXPECT_TRUE( l_sawDifferent ) << "RNG seems stuck or deterministic";
    }

    // Ensure random numbers are different across calls
    {
        const auto l_numberFirst = random::number::weak< size_t >();
        const auto l_numberSecond = random::number::weak< size_t >();

        EXPECT_NE( l_numberFirst, l_numberSecond );
    }

    // Ensure multiple calls return nonzero values
    for ( auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
        EXPECT_NE( random::number::weak< size_t >(), size_t{} );
    }

    for ( auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
        EXPECT_NE( random::number::weak< size_t >(), size_t{} );
    }
}

TEST( stdfunc, random$number$balanced ) {
    {
        // Integral range
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            int l_value = random::number::balanced( 1, 10 );
            EXPECT_GE( l_value, 1 );
            EXPECT_LE( l_value, 10 );
        }

        // Floating-point range
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            double l_value = random::number::balanced( 0.5, 2.5 );
            EXPECT_GE( l_value, 0.5 );
            EXPECT_LE( l_value, 2.5 );
        }

        // Different types
        auto l_intVal =
            random::number::balanced< int >( 5, 5 ); // degenerate range
        auto l_floatVal =
            random::number::balanced< float >( 1.0f, 1.0f ); // degenerate range
        EXPECT_EQ( l_intVal, 5 );
        EXPECT_FLOAT_EQ( l_floatVal, 1.0f );

        // Variability check (not all results should be identical)
        bool l_sawDifferent = false;
        int l_first = random::number::balanced( 1, 3 );
        for ( int l_i = 0; l_i < 50; ++l_i ) {
            if ( random::number::balanced( 1, 3 ) != l_first ) {
                l_sawDifferent = true;
                break;
            }
        }

        EXPECT_TRUE( l_sawDifferent ) << "RNG seems stuck or deterministic";
    }

    // Ensure random numbers are different across calls
    {
        const auto l_numberFirst = random::number::balanced< size_t >();
        const auto l_numberSecond = random::number::balanced< size_t >();

        EXPECT_NE( l_numberFirst, l_numberSecond );
    }

    // Ensure multiple calls return nonzero values
    for ( auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
        EXPECT_NE( random::number::balanced< size_t >(), size_t{} );
    }

    for ( auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
        EXPECT_NE( random::number::balanced< size_t >(), size_t{} );
    }
}

TEST( stdfunc, random$value ) {
    {
        // Test with vector<int>
        std::vector< int > l_vec{ 1, 2, 3, 4, 5 };
        for ( int l_i = 0; l_i < 100; ++l_i ) {
            int& l_ref = random::value( l_vec );
            EXPECT_TRUE( l_ref >= 1 && l_ref <= 5 );
        }

        // Test with std::array
        std::array< char, 3 > l_arr{ 'a', 'b', 'c' };
        for ( int l_i = 0; l_i < 100; ++l_i ) {
            char& l_ref = random::value( l_arr );
            EXPECT_TRUE( l_ref == 'a' || l_ref == 'b' || l_ref == 'c' );
        }

        // Reference semantics: modify element through returned ref
        std::vector< int > l_modVec{ 10, 20, 30 };
        int& l_picked = random::value( l_modVec );
        int l_old = l_picked;
        l_picked = 99; // modify through reference
        EXPECT_TRUE( std::ranges::find( l_modVec, 99 ) != l_modVec.end() );
        // Put it back so test is stable
        *std::ranges::find( l_modVec, 99 ) = l_old;

        // Degenerate case: container with 1 element
        std::vector< int > l_single{ 42 };
        for ( int l_i = 0; l_i < 10; ++l_i ) {
            EXPECT_EQ( random::value( l_single ), 42 );
        }

        // Empty container should assert (death test)
        std::vector< int > l_empty;
        EXPECT_DEATH( { random::value( l_empty ); }, "" );
    }

    // Const
    {
        // Reference semantics: modify element through returned ref
        const std::vector< int > l_modVec{ 10, 20, 30 };
        const int& l_picked = random::value( l_modVec );
        EXPECT_TRUE( std::ranges::find( l_modVec, l_picked ) !=
                     l_modVec.end() );

        // Degenerate case: container with 1 element
        const std::vector< int > l_single{ 42 };
        for ( int l_i = 0; l_i < 10; ++l_i ) {
            EXPECT_EQ( random::value( l_single ), 42 );
        }

        // Empty container should assert (death test)
        const std::vector< int > l_empty;
        EXPECT_DEATH( { random::value( l_empty ); }, "" );
    }
}

TEST( stdfunc, random$view ) {
    // ----- Setup -----
    std::vector< int > l_vec{ 1, 2, 3, 4, 5 };
    std::unordered_set< int > l_allowed( l_vec.begin(), l_vec.end() );

    // reseed deterministic engine & reset counter
    stdfunc::random::number::g_engine.seed( 12345u );

    // Construct view (should be lazy: no calls yet)
    auto l_v = random::view( l_vec );

    // Take a finite chunk and iterate; each element must be one of the
    // container values.
    auto l_taken = l_v | std::views::take( 100 );
    std::size_t l_saw = 0;
    for ( int l_value : l_taken ) {
        ++l_saw;
        EXPECT_TRUE( l_allowed.count( l_value ) )
            << "value produced by view is not present in container";
    }
    EXPECT_EQ( l_saw, 100u );

    // ----- Reference semantics test: modify via returned reference affects the
    // container ----- reseed & reset so sequence deterministic for the next
    // small test
    stdfunc::random::number::g_engine.seed( 42u );

    std::vector< int > l_modVec{ 10, 20, 30 };
    // get a single element view and take one element; the transform returns a
    // reference
    auto l_singleView = random::view( l_modVec ) | std::views::take( 1 );
    auto l_it = l_singleView.begin();
    ASSERT_NE( l_it, l_singleView.end() ); // sanity
    // Extract as reference (transform's callable returns a reference, so deref
    // should bind to it)
#if 0
    auto& l_ref = *l_it;
    // Modify through view reference and observe the change in the container
    int l_old = l_ref;
    l_ref = 9999;
    EXPECT_NE( std::ranges::find( l_modVec, 9999 ), l_modVec.end() )
        << "Modifying element returned by view must modify the underlying "
           "container";
    // restore so other tests are unaffected
    *std::ranges::find( l_modVec, 9999 ) = l_old;
#endif

    // ----- Single-element container: should always return that same element
    // -----
    stdfunc::random::number::g_engine.seed( 7u );
    std::vector< int > l_single{ 77 };
    auto l_singleAll = random::view( l_single ) | std::views::take( 10 );
    for ( int l_x : l_singleAll ) {
        EXPECT_EQ( l_x, 77 );
    }

    // ----- Empty container: debug builds assert(); test death in debug only
    // -----
#if 0
    std::vector< int > l_empty;
    // In debug builds the assert should trigger when calling view(empty)
    EXPECT_DEATH( { ( void )random::view( l_empty ); }, "" );
#endif
}

TEST( stdfunc, random$fill ) {
    // reseed RNG so test is deterministic
    stdfunc::random::number::g_engine.seed( 12345u );

    // ---- fill with min/max ----
    std::vector< int > l_vec( 100 );
    random::fill( l_vec, 10, 20 );
    for ( int l_v : l_vec ) {
        EXPECT_GE( l_v, 10 );
        EXPECT_LE( l_v, 20 );
    }

    // Degenerate case: min == max
    std::vector< int > l_singleVal( 5 );
    random::fill( l_singleVal, 42, 42 );
    for ( int l_v : l_singleVal ) {
        EXPECT_EQ( l_v, 42 );
    }

    // Different arithmetic type (double)
    std::vector< double > l_doubles( 50 );
    random::fill( l_doubles, 0.5, 1.5 );
    for ( double l_v : l_doubles ) {
        EXPECT_GE( l_v, 0.5 );
        EXPECT_LE( l_v, 1.5 );
    }

    // ---- fill with default distribution ----
    std::vector< int > l_vec2( 10 );
    random::fill( l_vec2 );
    // Values will depend on default distribution, just check they changed
    bool l_allZero = std::ranges::all_of(
        l_vec2, []( int _v ) -> bool { return ( _v == 0 ); } );
    EXPECT_FALSE( l_allZero )
        << "Default fill should not leave all elements as 0";

    // ---- check determinism with fixed seed ----
    stdfunc::random::number::g_engine.seed( 12345u );
    std::vector< int > l_seq1( 5 ), l_seq2( 5 );
    random::fill( l_seq1, 1, 100 );
    stdfunc::random::number::g_engine.seed( 12345u );
    random::fill( l_seq2, 1, 100 );
    EXPECT_EQ( l_seq1, l_seq2 )
        << "fill with same seed must produce identical sequence";
}

TEST( stdfunc, generateHash$weak ) {
    // Invalid inputs
    {
        // Valid buffer
        {
            // Non NULL terminated string
            {
                std::array l_buffer = { '0'_b };

                EXPECT_NE( hash::weak< size_t >( l_buffer ), size_t{} );
            }

            // NULL terminated string
            {
                std::array l_buffer = ""_bytes;

                EXPECT_NE( hash::weak< size_t >( l_buffer ), size_t{} );
            }
        }
    }

    // Valid buffer
    {
        // Ensure multiple calls return nonzero values
        {
            for ( const auto _index : std::views::iota( 1uz, 10'000uz ) ) {
                const size_t l_bufferLength = _index;

                std::vector< std::byte > l_buffer( l_bufferLength );

                stdfunc::random::fill( l_buffer );

                EXPECT_EQ( l_buffer.size(), l_bufferLength );

                const auto l_actualHash = hash::weak< size_t >( l_buffer );

                EXPECT_TRUE( l_actualHash );
            }
        }
    }

    {
        // -- Basic identical-input determinism --
        std::vector< std::byte > l_d = { std::byte{ 0 }, std::byte{ 1 },
                                         std::byte{ 2 }, std::byte{ 3 } };
        auto l_span = std::span< std::byte >( l_d );
        auto l_h1 = hash::weak< size_t >( l_span );
        auto l_h2 = hash::weak< size_t >( l_span );
        EXPECT_EQ( l_h1, l_h2 );

        // wrapper sanity: matches direct XXH32 call (note cast to size_t for
        // return type)
        EXPECT_EQ( l_h1, static_cast< size_t >(
                             XXH32( l_d.data(), l_d.size(),
                                    static_cast< unsigned >( 0x9e3779b1 ) ) ) );

        // -- Default seed equals explicit default seed --
        size_t l_hExplicitDefault = hash::weak< uint >( l_span );
        EXPECT_EQ( l_h1, l_hExplicitDefault );

        // -- Different seeds should (practically always) produce different
        // results --
        size_t l_hSeedDiff = hash::weak< uint >( l_span );
        EXPECT_NE( l_h1, l_hSeedDiff )
            << "Different seeds produced same hash — "
               "extremely unlikely but possible.";

        // -- Empty span behavior --
        std::vector< std::byte > l_empty;
        auto l_emptySpan = std::span< std::byte >( l_empty );
        auto l_he1 = hash::weak< size_t >( l_emptySpan );
        auto l_he2 = hash::weak< size_t >( l_emptySpan );
        EXPECT_EQ( l_he1, l_he2 );
        EXPECT_EQ( l_he1, static_cast< size_t >( XXH32(
                              l_empty.data(), l_empty.size(),
                              static_cast< unsigned >( 0x9e3779b1 ) ) ) );

        // -- Small change in data should (practically always) change the hash
        // --
        std::vector< std::byte > l_d2 = l_d;
        auto l_hOrig = hash::weak< size_t >( std::span< std::byte >( l_d2 ) );
        l_d2[ 2 ] = std::byte{ 0xFF };
        auto l_hChanged =
            hash::weak< size_t >( std::span< std::byte >( l_d2 ) );
        EXPECT_NE( l_hOrig, l_hChanged )
            << "Changing one byte produced same hash — extremely unlikely but "
               "possible.";

        // -- Hashing string content (ensure layout correct) --
        const std::string l_s = "hello, xxhash!";
        const auto l_bytesFromString =
            []( std::string_view _s ) -> std::vector< std::byte > {
            std::vector< std::byte > l_out( _s.size() );
            if ( !l_out.empty() ) {
                __builtin_memcpy( l_out.data(), _s.data(), _s.size() );
            }
            return ( l_out );
        };
        auto l_sbytes = l_bytesFromString( l_s );
        auto l_hs = hash::weak< size_t >( std::span< std::byte >( l_sbytes ) );
        // direct XXH32 of the original char data must match (no
        // reinterpretation errors)
        EXPECT_EQ( l_hs, static_cast< size_t >(
                             XXH32( l_s.data(), l_s.size(),
                                    static_cast< unsigned >( 0x9e3779b1 ) ) ) );

        // -- Larger data quick smoke test (no assertions beyond bounds) --
        std::vector< std::byte > l_large( 1024 );
        std::vector< unsigned char > l_tmp( l_large.size() );
        std::ranges::iota( l_tmp, 0u );

        std::ranges::transform( l_tmp, l_large.begin(),
                                []( unsigned char _c ) -> std::byte {
                                    return ( static_cast< std::byte >( _c ) );
                                } );
#if 0
        std::iota(
            std::bit_cast< unsigned char* >( l_large.data() ),
            std::bit_cast< unsigned char* >( l_large.data() + l_large.size() ),
            0u ); // fill with increasing bytes (note: UB if reinterpret_cast
                  // used with std::byte on write, but this is just to
                  // illustrate)
#endif
        // safer fill for std::byte:
        for ( size_t l_i = 0; l_i < l_large.size(); ++l_i )
            l_large[ l_i ] =
                std::byte( static_cast< unsigned char >( l_i & 0xFF ) );
        EXPECT_NO_FATAL_FAILURE( {
            volatile auto l_hLarge =
                hash::weak< size_t >( std::span< std::byte >( l_large ) );
            ( void )l_hLarge;
        } );
    }
}

TEST( stdfunc, generateHash$balanced ) {
    // Invalid inputs
    {
        // Valid buffer
        {
            // Non NULL terminated string
            {
                std::array l_buffer = { '0'_b };

                EXPECT_NE( hash::balanced< size_t >( l_buffer ), size_t{} );
            }

            // NULL terminated string
            {
                std::array l_buffer = ""_bytes;

                EXPECT_NE( hash::balanced< size_t >( l_buffer ), size_t{} );
            }
        }
    }

    // Valid buffer
    {
        // Ensure multiple calls return nonzero values
        {
            for ( const auto _index : std::views::iota( 1uz, 10'000uz ) ) {
                const size_t l_bufferLength = _index;

                std::vector< std::byte > l_buffer( l_bufferLength );

                stdfunc::random::fill( l_buffer );

                EXPECT_EQ( l_buffer.size(), l_bufferLength );

                const auto l_actualHash = hash::balanced< size_t >( l_buffer );

                EXPECT_TRUE( l_actualHash );
            }
        }
    }

    {
        // -- Basic identical-input determinism --
        std::vector< std::byte > l_d = { std::byte{ 0 }, std::byte{ 1 },
                                         std::byte{ 2 }, std::byte{ 3 } };
        auto l_span = std::span< std::byte >( l_d );
        auto l_h1 = hash::balanced< size_t >( l_span );
        auto l_h2 = hash::balanced< size_t >( l_span );
        EXPECT_EQ( l_h1, l_h2 );

        // wrapper sanity: matches direct XXH32 call (note cast to size_t for
        // return type)
        EXPECT_EQ( l_h1, static_cast< size_t >(
                             XXH32( l_d.data(), l_d.size(),
                                    static_cast< unsigned >( 0x9e3779b1 ) ) ) );

        // -- Default seed equals explicit default seed --
        auto l_hExplicitDefault =
            hash::balanced< size_t >( l_span, 0x9e3779b1 );
        EXPECT_EQ( l_h1, l_hExplicitDefault );

        // -- Different seeds should (practically always) produce different
        // results --
        auto l_hSeedDiff = hash::balanced< size_t >( l_span, 123456u );
        EXPECT_NE( l_h1, l_hSeedDiff )
            << "Different seeds produced same hash — "
               "extremely unlikely but possible.";

        // -- Empty span behavior --
        std::vector< std::byte > l_empty;
        auto l_emptySpan = std::span< std::byte >( l_empty );
        auto l_he1 = hash::balanced< size_t >( l_emptySpan );
        auto l_he2 = hash::balanced< size_t >( l_emptySpan );
        EXPECT_EQ( l_he1, l_he2 );
        EXPECT_EQ( l_he1, static_cast< size_t >( XXH32(
                              l_empty.data(), l_empty.size(),
                              static_cast< unsigned >( 0x9e3779b1 ) ) ) );

        // -- Small change in data should (practically always) change the hash
        // --
        std::vector< std::byte > l_d2 = l_d;
        auto l_hOrig =
            hash::balanced< size_t >( std::span< std::byte >( l_d2 ) );
        l_d2[ 2 ] = std::byte{ 0xFF };
        auto l_hChanged =
            hash::balanced< size_t >( std::span< std::byte >( l_d2 ) );
        EXPECT_NE( l_hOrig, l_hChanged )
            << "Changing one byte produced same hash — extremely unlikely but "
               "possible.";

        // -- Hashing string content (ensure layout correct) --
        const std::string l_s = "hello, xxhash!";
        const auto l_bytesFromString =
            []( std::string_view _s ) -> std::vector< std::byte > {
            std::vector< std::byte > l_out( _s.size() );
            if ( !l_out.empty() ) {
                __builtin_memcpy( l_out.data(), _s.data(), _s.size() );
            }
            return ( l_out );
        };
        auto l_sbytes = l_bytesFromString( l_s );
        auto l_hs =
            hash::balanced< size_t >( std::span< std::byte >( l_sbytes ) );
        // direct XXH32 of the original char data must match (no
        // reinterpretation errors)
        EXPECT_EQ( l_hs, static_cast< size_t >(
                             XXH32( l_s.data(), l_s.size(),
                                    static_cast< unsigned >( 0x9e3779b1 ) ) ) );

        // -- Larger data quick smoke test (no assertions beyond bounds) --
        std::vector< std::byte > l_large( 1024 );
        std::vector< unsigned char > l_tmp( l_large.size() );
        std::ranges::iota( l_tmp, 0u );

        std::ranges::transform( l_tmp, l_large.begin(),
                                []( unsigned char _c ) -> std::byte {
                                    return ( static_cast< std::byte >( _c ) );
                                } );
#if 0
        std::iota(
            std::bit_cast< unsigned char* >( l_large.data() ),
            std::bit_cast< unsigned char* >( l_large.data() + l_large.size() ),
            0u ); // fill with increasing bytes (note: UB if reinterpret_cast
                  // used with std::byte on write, but this is just to
                  // illustrate)
#endif
        // safer fill for std::byte:
        for ( size_t l_i = 0; l_i < l_large.size(); ++l_i )
            l_large[ l_i ] =
                std::byte( static_cast< unsigned char >( l_i & 0xFF ) );
        EXPECT_NO_FATAL_FAILURE( {
            volatile auto l_hLarge =
                hash::balanced< size_t >( std::span< std::byte >( l_large ) );
            ( void )l_hLarge;
        } );
    }
}

TEST( stdfunc, makeVariantContainer ) {
    {
        // Use std::vector (has defaulted allocator, so it works as a
        // template-template arg)
        auto l_v = makeVariantContainer< std::vector >(
            42, 3.14, 'c', std::string( "hello" ) );

        ASSERT_EQ( l_v.size(), 4u );

        // element 0 -> int
        EXPECT_TRUE( std::holds_alternative< int >( l_v[ 0 ] ) );
        EXPECT_EQ( std::get< int >( l_v[ 0 ] ), 42 );

        // element 1 -> double
        EXPECT_TRUE( std::holds_alternative< double >( l_v[ 1 ] ) );
        EXPECT_DOUBLE_EQ( std::get< double >( l_v[ 1 ] ), 3.14 );

        // element 2 -> char
        EXPECT_TRUE( std::holds_alternative< char >( l_v[ 2 ] ) );
        EXPECT_EQ( std::get< char >( l_v[ 2 ] ), 'c' );

        // element 3 -> std::string
        EXPECT_TRUE( std::holds_alternative< std::string >( l_v[ 3 ] ) );
        EXPECT_EQ( std::get< std::string >( l_v[ 3 ] ), "hello" );
    }

    {
        // Choose distinct argument types so std::variant alternative types are
        // unique.
        // This exercises the template< typename, size_t > overload
        // (std::array).
        auto l_a = makeVariantContainer< std::array >( 1, 2.0f, 'z' );

        static_assert( std::is_same_v<
                       decltype( l_a ),
                       std::array< std::variant< int, float, char >, 3 > > );

        ASSERT_EQ( l_a.size(), 3u );

        EXPECT_TRUE( std::holds_alternative< int >( l_a[ 0 ] ) );
        EXPECT_EQ( std::get< int >( l_a[ 0 ] ), 1 );

        EXPECT_TRUE( std::holds_alternative< float >( l_a[ 1 ] ) );
        EXPECT_FLOAT_EQ( std::get< float >( l_a[ 1 ] ), 2.0f );

        EXPECT_TRUE( std::holds_alternative< char >( l_a[ 2 ] ) );
        EXPECT_EQ( std::get< char >( l_a[ 2 ] ), 'z' );
    }

    {
        // This verifies constexpr usage for the std::array overload.
        // Types must be literal/constexpr-friendly for this to work.
        constexpr auto l_carr =
            makeVariantContainer< std::array >( 10, 20.5, 'q' );

        // Static checks at compile time:
        static_assert( std::holds_alternative< int >( l_carr[ 0 ] ) );
        static_assert( std::get< int >( l_carr[ 0 ] ) == 10 );

        static_assert( std::holds_alternative< double >( l_carr[ 1 ] ) );
        static_assert( std::get< double >( l_carr[ 1 ] ) == 20.5 );

        static_assert( std::holds_alternative< char >( l_carr[ 2 ] ) );
        static_assert( std::get< char >( l_carr[ 2 ] ) == 'q' );

        // Also runtime check just to be explicit in the test output if
        // something fails.
        EXPECT_EQ( std::get< int >( l_carr[ 0 ] ), 10 );
        EXPECT_DOUBLE_EQ( std::get< double >( l_carr[ 1 ] ), 20.5 );
        EXPECT_EQ( std::get< char >( l_carr[ 2 ] ), 'q' );
    }
}

TEST( stdfunc, filesystem$getApplicationDirectoryAbsolutePath ) {
    auto l_path = filesystem::getApplicationDirectoryAbsolutePath();

    EXPECT_NE( l_path, std::nullopt );
}

TEST( stdfunc, compress$decompress ) {
    {
        const std::string l_original = "hello, compress world!";
        auto l_compressedOpt = compress::text( l_original, /*level=*/1 );
        ASSERT_TRUE( l_compressedOpt.has_value() )
            << "compress::text failed for simple input";

        auto l_decompressedOpt = decompress::text( *l_compressedOpt );
        ASSERT_TRUE( l_decompressedOpt.has_value() )
            << "decompress::text failed for simple input";
        EXPECT_EQ( *l_decompressedOpt, l_original );
    }

    {
        const std::string l_original = "";
        auto l_compressedOpt = compress::text( l_original );
        ASSERT_FALSE( l_compressedOpt.has_value() );
#if 0
        auto l_decompressedOpt = decompress::text( *l_compressedOpt );
        ASSERT_FALSE( l_decompressedOpt.has_value() );
#endif
    }

    {
        // Repetitive data compresses well — test both compression and
        // correctness.
        std::string l_original;
        l_original.reserve( 5000 );
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            l_original += "abcabcabcabcabcabc";
        }

        auto l_compressedOpt = compress::text( l_original, /*level=*/2 );
        ASSERT_TRUE( l_compressedOpt.has_value() );
        // We expect compression to reduce size for repetitive text (very
        // likely).
        EXPECT_LT( l_compressedOpt->size(), l_original.size() );

        auto l_decompressedOpt = decompress::text( *l_compressedOpt );
        ASSERT_TRUE( l_decompressedOpt.has_value() );
        EXPECT_EQ( *l_decompressedOpt, l_original );
    }

    {
        const std::string l_original = "detect corruption test";
        auto l_compressedOpt = compress::text( l_original );
        ASSERT_TRUE( l_compressedOpt.has_value() );

        // Corrupt a byte in the compressed output (flip bits). This should
        // normally make decompression fail.
        std::string l_corrupted = *l_compressedOpt;
        if ( !l_corrupted.empty() ) {
            l_corrupted[ 0 ] = static_cast< char >( l_corrupted[ 0 ] ^ 0xFF );
        } else {
            // If compressed representation is empty (weird), append a corrupted
            // byte to force failure.
            l_corrupted.push_back( '\xFF' );
        }

        auto l_decompressedOpt = decompress::text( l_corrupted );
        EXPECT_FALSE( l_decompressedOpt.has_value() )
            << "decompress::text returned a value for corrupted input "
               "(expected failure)";
    }

    {
        // Build binary data with nulls and all byte values
        const size_t l_n = 512;
        std::vector< std::byte > l_original( l_n );
        for ( size_t l_i = 0; l_i < l_n; ++l_i ) {
            l_original[ l_i ] =
                std::byte( static_cast< unsigned char >( l_i & 0xFF ) );
        }

        auto l_compressedOpt =
            compress::data( std::span< std::byte >( l_original ), /*level=*/3 );
        ASSERT_TRUE( l_compressedOpt.has_value() )
            << "compress::data failed for binary input";

        auto l_decompressedOpt = decompress::data(
            std::span< std::byte >( *l_compressedOpt ), l_original.size() );
        ASSERT_TRUE( l_decompressedOpt.has_value() )
            << "decompress::data failed for binary input";
        EXPECT_EQ( l_decompressedOpt->size(), l_original.size() );
        EXPECT_EQ( *l_decompressedOpt, l_original );
    }

    {
        std::vector< std::byte > l_original;
        auto l_compressedOpt =
            compress::data( std::span< std::byte >( l_original ) );
        ASSERT_FALSE( l_compressedOpt.has_value() );
#if 0
        auto l_decompressedOpt =
            decompress::data( std::span< std::byte >( *l_compressedOpt ), 0 );
        ASSERT_FALSE( l_decompressedOpt.has_value() );
#endif
    }

    {
        const size_t l_n = 128;
        std::vector< std::byte > l_original( l_n );
        for ( size_t l_i = 0; l_i < l_n; ++l_i ) {
            l_original[ l_i ] =
                std::byte( static_cast< unsigned char >( l_i ) );
        }

        auto l_compressedOpt =
            compress::data( std::span< std::byte >( l_original ), /*level=*/1 );
        ASSERT_TRUE( l_compressedOpt.has_value() );

        // Corrupt compressed bytes
        std::vector< std::byte > l_corrupted = *l_compressedOpt;
        if ( !l_corrupted.empty() ) {
            l_corrupted[ 0 ] = std::byte( static_cast< unsigned char >(
                static_cast< unsigned char >( l_corrupted[ 0 ] ) ^ 0xFFu ) );
        } else {
            l_corrupted.push_back( std::byte{ 0xFF } );
        }

        auto l_decompressedOpt = decompress::data(
            std::span< std::byte >( l_corrupted ), l_original.size() );
        EXPECT_FALSE( l_decompressedOpt.has_value() )
            << "decompress::data returned a value for corrupted input "
               "(expected failure)";
    }

    {
        const size_t l_n = 300;
        std::vector< std::byte > l_original( l_n );
        for ( size_t l_i = 0; l_i < l_n; ++l_i ) {
            l_original[ l_i ] =
                std::byte( static_cast< unsigned char >( l_i * 31 ) );
        }

        auto l_c1 =
            compress::data( std::span< std::byte >( l_original ), /*level=*/1 );
        auto l_c5 =
            compress::data( std::span< std::byte >( l_original ), /*level=*/5 );
        auto l_c9 =
            compress::data( std::span< std::byte >( l_original ), /*level=*/9 );

        ASSERT_TRUE( l_c1.has_value() );
        ASSERT_TRUE( l_c5.has_value() );
        ASSERT_TRUE( l_c9.has_value() );

        // All should roundtrip correctly
        auto l_d1 = decompress::data( std::span< std::byte >( *l_c1 ),
                                      l_original.size() );
        ASSERT_TRUE( l_d1.has_value() );
        EXPECT_EQ( *l_d1, l_original );

        auto l_d5 = decompress::data( std::span< std::byte >( *l_c5 ),
                                      l_original.size() );
        ASSERT_TRUE( l_d5.has_value() );
        EXPECT_EQ( *l_d5, l_original );

        auto l_d9 = decompress::data( std::span< std::byte >( *l_c9 ),
                                      l_original.size() );
        ASSERT_TRUE( l_d9.has_value() );
        EXPECT_EQ( *l_d9, l_original );

        // Compressed sizes might differ — usually higher level gives smaller or
        // equal compressed size
        EXPECT_LE( l_c9->size(), l_c1->size() );
    }
}

struct person {
    int id{};
    double salary{};
    std::string name{};
};

struct empty {};

enum class color : int8_t { red = 1, green = 2, blue = 10 };

// -----------------------------
// Compile-time checks
// -----------------------------
static_assert( meta::is_reflectable< person > );
static_assert( meta::reflect_t< person >::size == 3,
               "Person size should be 3" );
static_assert( meta::reflect_t< person >::keys[ 0 ] == "id" );
static_assert( meta::hasMemberWithName< person >( "id" ) );
static_assert( !meta::hasMemberWithName< person >( "not_a_field" ) );
static_assert( meta::is_reflectable< empty > );
static_assert( meta::reflect_t< empty >::size == 0, "Empty size should be 0" );
static_assert( !meta::hasMemberWithName< empty >( "id" ) );
static_assert( !meta::hasMemberWithName< empty >( "not_a_field" ) );
#if 0
static_assert( meta::hasMemberWithName< Color >( "Green" ) );
#endif

// -----------------------------
// GoogleTest cases
// -----------------------------

TEST( MetaReflectTests, IterateInstance_Person ) {
    person l_p{};
    l_p.id = 42;
    l_p.salary = 1234.56;
    l_p.name = "alice";

    std::vector< std::string > l_seen;
    meta::iterateStructTopMostFields(
        l_p, [ &l_seen ]( auto&& _field ) -> auto {
            using t_t = std::decay_t< decltype( _field ) >;
            if constexpr ( std::is_same_v< t_t, int > ) {
                l_seen.push_back( "int:" + std::to_string( _field ) );
            } else if constexpr ( std::is_same_v< t_t, double > ) {
                l_seen.emplace_back( "double" );
            } else if constexpr ( std::is_same_v< t_t, std::string > ) {
                l_seen.push_back( std::string( "string:" ) + _field );
            } else {
                l_seen.emplace_back( "other" );
            }
        } );

    ASSERT_EQ( l_seen.size(), 3u );
    EXPECT_EQ( l_seen[ 0 ], "int:42" );
    EXPECT_EQ( l_seen[ 1 ], "double" );
    EXPECT_EQ( l_seen[ 2 ], "string:alice" );
}

TEST( MetaReflectTests, IterateType_Person ) {
    std::vector< std::string > l_seen;
    meta::iterateStructTopMostFields< person >(
        [ &l_seen ]( auto&& _field ) -> void {
            using t_t = std::decay_t< decltype( _field ) >;
            if constexpr ( std::is_same_v< t_t, int > ) {
                l_seen.emplace_back( "int" );
            } else if constexpr ( std::is_same_v< t_t, double > ) {
                l_seen.emplace_back( "double" );
            } else if constexpr ( std::is_same_v< t_t, std::string > ) {
                l_seen.emplace_back( "string" );
            } else {
                l_seen.emplace_back( "other" );
            }
        } );

    ASSERT_EQ( l_seen.size(), 3u );
    EXPECT_EQ( l_seen[ 0 ], "int" );
    EXPECT_EQ( l_seen[ 1 ], "double" );
    EXPECT_EQ( l_seen[ 2 ], "string" );
}

#if 0
TEST( MetaReflectTests, IterateUnion_TopLevel ) {
    union MyUnion {
        int i;
        float f;
    };

    MyUnion l_u;
    l_u.i = 7;
    std::vector< std::string > l_seen;
    meta::iterateStructTopMostFields(
        l_u, [ &l_seen ]( auto&& _field ) -> auto {
            using t_t = std::decay_t< decltype( _field ) >;
            if constexpr ( std::is_same_v< t_t, int > ) {
                l_seen.emplace_back( "int" );
            } else if constexpr ( std::is_same_v< t_t, float > ) {
                l_seen.emplace_back( "float" );
            } else {
                l_seen.emplace_back( "other" );
            }
        } );
    ASSERT_EQ( l_seen.size(), 2u );
    EXPECT_EQ( l_seen[ 0 ], "int" );
    EXPECT_EQ( l_seen[ 1 ], "float" );
}
#endif

#if 0
TEST( MetaReflectTests, EnumIteration_TypeAndInstance ) {
    // Type-based
    std::vector< int > l_ids;
    meta::iterateStructTopMostFields< Color >( [ &l_ids ]( auto&& _e ) -> auto {
        l_ids.push_back( static_cast< int >( _e ) );
    } );
    ASSERT_EQ( l_ids.size(), 3u );
    EXPECT_EQ( l_ids[ 0 ], 1 );
    EXPECT_EQ( l_ids[ 1 ], 2 );
    EXPECT_EQ( l_ids[ 2 ], 10 );

    // Instance-based: our mock also iterates enumerators
    Color l_c = Color::Green;
    std::vector< int > l_ids2;
    meta::iterateStructTopMostFields( l_c, [ &l_ids2 ]( auto&& _e ) -> auto {
        l_ids2.push_back( static_cast< int >( _e ) );
    } );
    ASSERT_EQ( l_ids2.size(), 3u );
    EXPECT_EQ( l_ids2[ 0 ], 1 );
    EXPECT_EQ( l_ids2[ 1 ], 2 );
    EXPECT_EQ( l_ids2[ 2 ], 10 );
}
#endif

TEST( MetaReflectTests, reflect_t_and_getName ) {
    using r_t = meta::reflect_t< person >;
    static_assert( r_t::size == 3u );
    static_assert( r_t::keys[ 0 ] == std::string_view( "id" ) );

    SUCCEED();
}
