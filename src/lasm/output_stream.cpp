#include "lasm/output_stream.hpp"
#include "clz.hpp"
#include "dragonbox/dragonbox_to_chars.h"
#include "int2strt.hpp"
#include "log.hpp"
#include <cstdio>
#include <cstdlib>

inline int Lasm::OutputStream::digit_count(u32 x) {
    // clang-format off
    static u64 table[] = {
     4294967296,    8589934582,   8589934582,   8589934582, 
     12884901788,  12884901788,  12884901788,  17179868184, 
     17179868184,  17179868184, 21474826480, 21474826480,
    21474826480, 21474826480, 25769703776, 25769703776, 
    25769703776, 30063771072, 30063771072, 30063771072, 
    34349738368, 34349738368, 34349738368, 34349738368, 
    38554705664, 38554705664, 38554705664, 41949672960,
    41949672960, 41949672960, 42949672960, 42949672960
    };
    // clang-format on
    return (x + table[CLZ32(x | 1) ^ 31]) >> 32;
}

inline const char* Lasm::OutputStream::digits2(size_t value) {
    // clang-format off
    alignas(2) static const char data[] = 
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";
    // clang-format on
    return &data[value * 2];
}

inline const char* Lasm::OutputStream::hex2(size_t value) {
    // clang-format off
    alignas(2) static const char data[] = 
    "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F"
    "202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F"
    "404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F"
    "606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F"
    "808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9F"
    "A0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
    "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
    "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
    // clang-format on
    return &data[value * 2];
}

Lasm::OutputStream::OutputStream() {
    u8* buf = static_cast<u8*>(malloc(INITIAL_CAPACITY));
    if (buf == nullptr) {
        LOGF("failed to allocate buffer for filewriter");
    }
    m_start = buf;
    m_cursor = buf;
    m_end = buf + INITIAL_CAPACITY;
}

Lasm::OutputStream::~OutputStream() {
    if (m_start) {
        free(m_start);
        m_start = m_cursor = m_end = nullptr;
    }
}

void Lasm::OutputStream::write(const void* src, size_t size) {
    memcpy(m_cursor, src, size);
    m_cursor += size;
}

void Lasm::OutputStream::write(i32 x) {
    u32 abs_value = static_cast<u32>(x);
    bool negative = x < 0;
    if (negative) {
        abs_value = 0 - abs_value;
    }
    int n = digit_count(abs_value);
    u8* ncur = m_cursor;
    if (negative) {
        *ncur++ = '-';
    }
    ncur += n;
    m_cursor = ncur;
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
}

void Lasm::OutputStream::write(u32 x) {
    u32 abs_value = x;
    int n = digit_count(abs_value);
    u8* ncur = m_cursor;
    ncur += n;
    m_cursor = ncur;
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
}

void Lasm::OutputStream::writeHex(u8 x) {
    write("0x");
    write(hex2(x), 2);
}

void Lasm::OutputStream::saveTo(char* path) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        LOGE("failed to open file assebly won't be saved\n");
        return;
    }

    size_t size = (size_t)(m_cursor - m_start);

    fwrite(m_start, 1, size, file);

    fclose(file);
}

void Lasm::OutputStream::reserve(size_t byteCount) {
    if (m_cursor + byteCount > m_end) {
        size_t newsize = (m_end - m_start) * 2;
        size_t usedsize = m_cursor - m_start;
        while (newsize < usedsize + byteCount) {
            newsize *= 2;
        }
        u8* start = (u8*)realloc(m_start, newsize);
        if (start == nullptr) {
            LOGF("failed to reallocate output buffer");
        }
        m_start = start;
        m_cursor = start + usedsize;
        m_end = start + newsize;
    }
}

void Lasm::OutputStream::write(u8 byte, size_t count) {
    memset(m_cursor, byte, count);
    m_cursor += count;
}

void Lasm::OutputStream::writeHex(u32 x) {
    write("0x");
    write(hex2(x >> 24), 2);
    write(hex2((x >> 16) & 0xFF), 2);
    write(hex2((x >> 8) & 0xFF), 2);
    write(hex2(x & 0xFF), 2);
}

void Lasm::OutputStream::writeHex9(u32 x) {
    write("0x");
    memset(m_cursor, '0' + ((x >> 8) & 1), 1);
    m_cursor += 1;
    write(hex2(x & 0xFF), 2);
}

void Lasm::OutputStream::writeEscape(const void* src, size_t size) {
    const u8* p = static_cast<const u8*>(src);
    u8* out = m_cursor;

    for (size_t i = 0; i < size;) {
        size_t start = i;
        while (i < size) {
            const u8 c = p[i];
            if (c == '\\' || c == '"' || c == '\'' || c == '\n' || c == '\r' || c == '\t' || c < 0x20 || c >= 0x7F) {
                break;
            }
            ++i;
        }
        const size_t count = i - start;
        if (count) {
            memcpy(out, p + start, count);
            out += count;
        }
        if (i == size)
            break;

        const u8 c = p[i++];

        switch (c) {
                // clang-format off
            case '\\': *out++ = '\\'; *out++ = '\\'; break;
            case '"' : *out++ = '\\'; *out++ = '"' ; break;
            case '\'': *out++ = '\\'; *out++ = '\''; break;
            case '\n': *out++ = '\\'; *out++ = 'n' ; break;
            case '\r': *out++ = '\\'; *out++ = 'r' ; break;
            case '\t': *out++ = '\\'; *out++ = 't' ; break;
            // clang-format on
            default:
                *out++ = '\\';
                *out++ = 'x';
                memcpy(out, hex2(c), 2);
                out += 2;
                break;
        }
    }

    m_cursor = out;
}

void Lasm::OutputStream::write(f64 x) {
    m_cursor = reinterpret_cast<u8*>(jkj::dragonbox::to_chars_n(x, reinterpret_cast<char*>(m_cursor)));
}

// 0x 1F FF FF FF FF FF FD
//    1   2  3  4  5  6  7
void Lasm::OutputStream::writeHex56(u64 x) {
    write("0x");
    write(hex2((x >> 48) & 0xFF), 2);
    write(hex2((x >> 40) & 0xFF), 2);
    write(hex2((x >> 32) & 0xFF), 2);
    write(hex2((x >> 24) & 0xFF), 2);
    write(hex2((x >> 16) & 0xFF), 2);
    write(hex2((x >> 8) & 0xFF), 2);
    write(hex2(x & 0xFF), 2);
}

void Lasm::OutputStream::writeHex(u64 x) {
    write("0x");
    write(hex2(x >> 56), 2);
    write(hex2((x >> 48) & 0xFF), 2);
    write(hex2((x >> 40) & 0xFF), 2);
    write(hex2((x >> 32) & 0xFF), 2);
    write(hex2((x >> 24) & 0xFF), 2);
    write(hex2((x >> 16) & 0xFF), 2);
    write(hex2((x >> 8) & 0xFF), 2);
    write(hex2(x & 0xFF), 2);
}

void Lasm::OutputStream::writeFast(int value) {
    size_t sz = 0;
    if (value > 9) {
        if (value > 99) {
            sz = 3;
        } else {
            sz = 2;
        }
    } else {
        sz = 1;
    }
    memcpy(m_cursor, (void*)(int2strt + (value) * 3), sz);
    m_cursor += sz;
}