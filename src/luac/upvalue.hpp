#pragma once
#include "luac/array.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"
namespace Luac {
    class Upvalue {
    public:
        u8 m_instack = 0;
        u8 m_idx = 0;
        static void loadArray(Luac::InputStream* stream, Array<Upvalue>* parr);
        static void loadArray(Luac::InputStream* stream, Upvalue* upvalues, size_t count);
        static void printArray(Lasm::OutputStream* stream, Array<Upvalue>* parr);
        void print(Lasm::OutputStream* stream, size_t index = 0);
    };
} // namespace Luac
