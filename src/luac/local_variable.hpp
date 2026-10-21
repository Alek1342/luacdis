#pragma once
#include "luac/array.hpp"
#include "luac/string.hpp"
#include "types.hpp"

namespace Luac {
    class LocalVariable {
    public:
        String* m_varname = new String;
        i32 m_startpc = 0;
        i32 m_endpc = 0;
        static void loadArray(Luac::InputStream* stream, Array<LocalVariable>* parr);
        static void printArray(Lasm::OutputStream* stream, Array<LocalVariable>* parr);
        void print(Lasm::OutputStream* stream);
        ~LocalVariable();
    };
} // namespace Luac
