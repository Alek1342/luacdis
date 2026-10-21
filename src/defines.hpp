#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
    #define LUACDIS_INLINE __forceinline
#elif defined(__clang__) || defined(__GNUC__)
    #define LUACDIS_INLINE inline __attribute__((always_inline))
#else
    #define LUACDIS_INLINE inline
#endif
