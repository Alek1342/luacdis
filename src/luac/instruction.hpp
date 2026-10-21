#pragma once
#include "luac/array.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Instruction {
    public:
        u32 value;
        static void loadArray(Luac::InputStream* stream, Array<Instruction>* parr);
        static void loadArray(Luac::InputStream* stream, Instruction* instructions, size_t count);
        static void printArray(Lasm::OutputStream* stream, Array<Instruction>* parr);
        static void printArray(Lasm::OutputStream* stream, Instruction* instructions, size_t count);
    };
} // namespace Luac