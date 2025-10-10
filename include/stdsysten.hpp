#pragma once

// TODO: Add support for 64bits
#if defined( __i386__ )

// TODO: SUpport All argument amount
// TODO: Support return values
namespace system {

using call_t = enum class call : uint8_t {
    exit = 1,
    read = 3,
    write = 4,
    ioctl = 54,
};

using callUnderlying_t = std::underlying_type_t< call_t >;

template < typename... Arguments >
    requires( sizeof...( Arguments ) <= 3 &&
              ( std::convertible_to< Arguments, uintptr_t > && ... ) )
FORCE_INLINE void syscall( call_t _systemCall, Arguments... _arguments ) {
    const auto l_systemCall = static_cast< uint32_t >( _systemCall );

    constexpr size_t l_amount = sizeof...( Arguments );

    std::array l_arguments = { static_cast< uintptr_t >( _arguments )... };

    if constexpr ( l_amount == 0 ) {
        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall )
                          : "memory", "cc" );

    } else if constexpr ( l_amount == 1 ) {
        const uintptr_t l_ebx = l_arguments[ 0 ];

        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall ), "b"( l_ebx )
                          : "memory", "cc" );

    } else if constexpr ( l_amount == 2 ) {
        const uintptr_t l_ebx = l_arguments[ 0 ];
        const uintptr_t l_ecx = l_arguments[ 1 ];

        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall ), "b"( l_ebx ), "c"( l_ecx )
                          : "memory", "cc" );

    } else if constexpr ( l_amount == 3 ) {
        const uintptr_t l_ebx = l_arguments[ 0 ];
        const uintptr_t l_ecx = l_arguments[ 1 ];
        const uintptr_t l_edx = l_arguments[ 2 ];

        __asm__ volatile( "int $0x80"
                          :
                          : "a"( l_systemCall ), "b"( l_ebx ), "c"( l_ecx ),
                            "d"( l_edx )
                          : "memory", "cc" );
    }
}

} // namespace system

#endif
