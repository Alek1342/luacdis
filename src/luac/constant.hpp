#pragma once
#include "luac/array.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Constant {
    public:
        enum Type : u8 { NIL = 0, BOOLEAN = 1, NUMBER = 3, STRING = 4, INT = 254 };

        u8 m_type = 0;
        void* m_value = nullptr;

        static void loadArray(Luac::InputStream* stream, Array<Constant>* parr);
        static void loadArray(Luac::InputStream* stream, Constant* constants, size_t count);
        void load(Luac::InputStream* stream);
        static void printArray(Lasm::OutputStream* stream, Array<Constant>* parr);
        void print(Lasm::OutputStream* stream, size_t index = 0);

        ~Constant();
    };
} // namespace Luac