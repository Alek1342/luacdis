#pragma once

#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Tail {
    private:
    public:
        u8* m_bytes = nullptr;
        size_t m_size = 0;
        void init(size_t size);
        void load(Luac::InputStream* stream);
        void dump(char* lasmPath);
        ~Tail();
    };
} // namespace Luac