#pragma once
#include "lasm/output_stream.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Number {
    private:
        static constexpr u64 DOUBLE_MANTISSA_MASK = (1ULL << 52) - 1;

    public:
        f64 m_fnum = 0;
        i32 m_inum = 0;
        bool m_isInt = false;
        bool m_isWhole = false;
        void load(Luac::InputStream* stream);
        bool isInt();
        bool isFloat();
        bool isWholeFloat();
        bool isWhole();
        void print(Lasm::OutputStream* stream);
    };

} // namespace Luac