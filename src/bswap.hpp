#pragma once

#if !defined(__clang__) && defined(_MSC_VER)
#include <intrin.h>
#endif

namespace detail {
#if defined(__clang__) || defined(__GNUC__)
#define BSWAP32(x) __builtin_bswap32(x)
#define BSWAP64(x) __builtin_bswap64(x)

#elif defined(_MSC_VER)
#define BSWAP32(x) _byteswap_ulong(x)
#define BSWAP64(x) _byteswap_uint64(x)

#else
#include "types.hpp"
    inline u32 bswap32(u32 x) {
        return ((x & 0x000000FFu) << 24) | ((x & 0x0000FF00u) << 8) | ((x & 0x00FF0000u) >> 8) |
               ((x & 0xFF000000u) >> 24);
    }

    inline u64 bswap64(u64 x) {
        return ((x & 0x00000000000000FFULL) << 56) | ((x & 0x000000000000FF00ULL) << 40) |
               ((x & 0x0000000000FF0000ULL) << 24) | ((x & 0x00000000FF000000ULL) << 8) |
               ((x & 0x000000FF00000000ULL) >> 8) | ((x & 0x0000FF0000000000ULL) >> 24) |
               ((x & 0x00FF000000000000ULL) >> 40) | ((x & 0xFF00000000000000ULL) >> 56);
    }
#define BSWAP32(x) detail::bswap32(x)
#define BSWAP64(x) detail::bswap64(x)

#endif
} // namespace detail