#pragma once
#include "lasm/output_stream.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class String {
    private:
        static size_t m_hugeCount;

    public:
        u8* m_bytes = nullptr;
        size_t m_size = 0;
        u32 m_unused = 0;
        void init(u8* bytes, size_t size, u32 unused = 0);
        void load(Luac::InputStream* stream);
        virtual void print(Lasm::OutputStream* stream);
        virtual ~String();
    };
} // namespace Luac
