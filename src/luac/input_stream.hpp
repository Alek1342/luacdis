#pragma once
#include "types.hpp"

namespace Luac {
    class Header;
    class InputStream {
    public:
        Header* m_luacHeader = nullptr;
        u8* m_start = nullptr;
        u8* m_cursor = nullptr;
        u8* m_end = nullptr;

        ~InputStream();

        bool request(size_t byteCount) const;

        u8 getByte();

        u32 get32();

        u64 get64();

        void getBuffer(size_t size, void* buffer);

        void loadFromFile(char* path);
    };
} // namespace Luac