#pragma once
#include "lasm/output_stream.hpp"
#include "luac/array.hpp"
#include "luac/constant.hpp"
#include "luac/input_stream.hpp"
#include "luac/instruction.hpp"
#include "luac/lineinfo.hpp"
#include "luac/local_variable.hpp"
#include "luac/source.hpp"
#include "luac/upvalue.hpp"
#include "luac/upvaluename.hpp"
#include "types.hpp"

namespace Luac {
    class Prototype {
    public:
        i32 m_linedefined;
        i32 m_lastlinedefined;
        u8 m_numparams;
        u8 m_is_vararg;
        u8 m_maxstacksize;
        Array<Instruction> m_code; // length >= 0
        Array<Constant> m_constants;
        Array<Prototype> m_functions;
        Array<Upvalue> m_upvalues;
        Source m_source;
        Array<Lineinfo> m_lineinfo; // length >= 0
        Array<LocalVariable> m_locvars;
        Array<Upvaluename> m_upvalnames;
        void load(Luac::InputStream* stream);
        static void loadArray(Luac::InputStream* stream, Array<Prototype>* parr);
        static void loadArray(Luac::InputStream* stream, Prototype* p, size_t count);
        void load(Luac::InputStream* stream, size_t count);
        void loadPHeader(Luac::InputStream* stream);
        void print(Lasm::OutputStream* stream);
        static void printArray(Lasm::OutputStream* stream, Prototype* p, size_t count);
        static void printArray(Lasm::OutputStream* stream, Array<Prototype>* parr);
        void printPHeader(Lasm::OutputStream* stream, size_t fnum);
    };
} // namespace Luac