#pragma once

#if !defined(__clang__) && defined(_MSC_VER)
#include <intrin.h>
#endif

namespace detail {
#if defined(__clang__) || defined(__GNUC__)
#define CLZ32(x) __builtin_clz(x)
#define CLZ64(x) __builtin_clzll(x)
#elif defined(_MSC_VER)
#pragma intrinsic(_BitScanReverse)
#ifdef _WIN64
#pragma intrinsic(_BitScanReverse64)
#endif
#include "types.hpp"
    inline int clz32(u32 x) {
        unsigned long r = 0;
        _BitScanReverse(&r, x);
        return 31 ^ static_cast<int>(r);
    }
    #define CLZ32(n) detail::clz32(n)

    inline int clz64(u64 x) {
        unsigned long r = 0;
    #ifdef _WIN64
        _BitScanReverse64(&r, x);
    #else
        // Scan the high 32 bits.
        if (_BitScanReverse(&r, static_cast<u32>(x >> 32)))
            return 63 ^ static_cast<int>(r + 32);
        // Scan the low 32 bits.
        _BitScanReverse(&r, static_cast<u32>(x));
    #endif
        return 63 ^ static_cast<int>(r);
    }
    #define CLZ64(n) detail::clz64(n)

#else

#endif
} // namespace detail