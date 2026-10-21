#pragma once
#include "lasm/output_stream.hpp"
#include "luac/array.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Lineinfo {
    public:
        i32 m_value;
        static void loadArray(Luac::InputStream* stream, Array<Lineinfo>* parr);
        static void printArray(Lasm::OutputStream* stream, Array<Lineinfo>* parr);
        void print(Lasm::OutputStream* stream);
    };
} // namespace Luac