#include "clz.hpp"
#include "log.hpp"
#include "types.hpp"
#include <cstdlib>
#include <cstring>

class BufferWriter {
private:
    inline int digit_count(u32 x) {
        static u64 table[] = {4294967296,  8589934582,  8589934582,  8589934582,  12884901788, 12884901788, 12884901788,
                              17179868184, 17179868184, 17179868184, 21474826480, 21474826480, 21474826480, 21474826480,
                              25769703776, 25769703776, 25769703776, 30063771072, 30063771072, 30063771072, 34349738368,
                              34349738368, 34349738368, 34349738368, 38554705664, 38554705664, 38554705664, 41949672960,
                              41949672960, 41949672960, 42949672960, 42949672960};
        return (x + table[CLZ32(x | 1) ^ 31]) >> 32;
    }
    // Converts value in the range [0, 100) to a string.
    // GCC generates slightly better code when value is pointer-size.
    inline const char* digits2(size_t value) {
        // Align data since unaligned access may be slower when crossing a
        // hardware-specific boundary.
        alignas(2) static const char data[] = "0001020304050607080910111213141516171819"
                                              "2021222324252627282930313233343536373839"
                                              "4041424344454647484950515253545556575859"
                                              "6061626364656667686970717273747576777879"
                                              "8081828384858687888990919293949596979899";
        return &data[value * 2];
    }

public:
    u8* start = nullptr;
    u8* cur = nullptr;
    u8* end = nullptr;
    BufferWriter() {
        void* buf = malloc(65536);
        if (buf == nullptr) {
            LOGF("failed to allocate buffer for filewriter");
        }
        start = (u8*)buf;
        cur = (u8*)buf;
        end = (u8*)buf + (size_t)65536;
    }
    ~BufferWriter() {
        free(start);
    }
    template <size_t N>
    void wcstr(const char (&src)[N]) {
        memcpy(cur, src, N - 1);
        cur += N - 1;
    }
    void wsstr(char* src, size_t sz) {
        memcpy(cur, src, sz);
        cur += sz;
    }
    void wi32(i32 x) {
        u32 abs_value = static_cast<u32>(x);
        bool negative = x < 0;
        if (negative) {
            abs_value = 0 - abs_value;
        }
        int n = digit_count(abs_value);
        u8* ncur = cur + n;
        cur = ncur;
        while (abs_value >= 100) {
            ncur -= 2;
            memcpy(ncur, digits2(static_cast<unsigned>(abs_value % 100)), 2);
            abs_value /= 100;
        }
        if (abs_value >= 10) {
            ncur -= 2;
            memcpy(ncur, digits2(static_cast<unsigned>(abs_value)), 2);
        } else {
            *--ncur = static_cast<u8>('0' + abs_value);
        }
        if (negative) {
            *--ncur = '-';
        }
    }
};