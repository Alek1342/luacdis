#pragma once
#include "lasm/output_stream.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Number {
    private:
        // INT_MIN is not integral cus ripped off code is buggy
        static constexpr i32 FLOAT_INTEGER_SENTINEL = INT32_MIN;
        static constexpr f64 INTEGER_TYPE_SENTINEL = 0.5;
        static constexpr i32 DEFAULT_INT = 0;
        static constexpr f64 DEFAULT_FLOAT = 0;

    public:
        i32 m_inum = DEFAULT_INT;
        f64 m_fnum = DEFAULT_FLOAT;
        void load(Luac::InputStream* stream);
        bool isInt();
        bool isFloatIntegral();
        bool isFloat();
        bool isTreatedAsInt();
        void print(Lasm::OutputStream* stream);
    };

} // namespace Luac