#include "stdrandom.hpp"

#include <cstddef>
#include <random>

namespace stdfunc::random {

#if 0
// TODO: Improve
// Seconds from midnight
const size_t g_compilationTimeAsSeed =
    ( ( ( ( ( __TIME__[ 0 ] - '0' ) * 10 ) + ( __TIME__[ 1 ] - '0' ) ) *
        3600 ) +
      ( ( ( ( __TIME__[ 3 ] - '0' ) * 10 ) + ( __TIME__[ 4 ] - '0' ) ) * 60 ) +
      ( ( ( __TIME__[ 6 ] - '0' ) * 10 ) + ( __TIME__[ 7 ] - '0' ) ) );
#endif

namespace number {

thread_local engine_t g_engine{
#if defined( STDFUNC_RANDOM_CONSTEXPR )

    g_goldenRatioSeed

#else

    std::random_device{}()

#endif
};

} // namespace number

} // namespace stdfunc::random
