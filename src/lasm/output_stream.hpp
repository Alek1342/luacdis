#pragma once

#include "lasm/print_state.hpp"
#include "types.hpp"
#include <cstring>

namespace Lasm {
    class OutputStream {
    private:
        static constexpr size_t INITIAL_CAPACITY = 65536;
        inline int digit_count(u32 x);
        inline const char* digits2(size_t value);
        inline const char* hex2(size_t value);

    public:
        Lasm::PrintState* m_printState = nullptr;
        size_t m_indent = 0;
        u8* m_start = nullptr;
        u8* m_cursor = nullptr;
        u8* m_end = nullptr;

        OutputStream();
        ~OutputStream();

        void write(const void* src, size_t size);
        void write(u8 byte, size_t count);

        void write(i32 x);
        void write(u32 x);
        void writeFast(int value);
        void write(f64 x);

        void writeHex(u8 x);
        void writeHex9(u32 x);
        void writeHex(u32 x);
        void writeHex(u64 x);
        void writeHex56(u64 x);

        void writeEscape(const void* src, size_t size);

        void saveTo(char* path);

        void reserve(size_t byteCount);

        template <size_t N>
        void write(const char (&src)[N]) {
            memcpy(m_cursor, src, N - 1);
            m_cursor += N - 1;
        }
    };

} // namespace Lasm