#pragma once
#include "luac/array.hpp"
#include "luac/input_stream.hpp"
#include "luac/string.hpp"

namespace Luac {
    class Upvaluename : public String {
    public:
        static void loadArray(Luac::InputStream* stream, Array<Upvaluename>* parr);
        static void printArray(Lasm::OutputStream* stream, Array<Upvaluename>* parr);
        void print(Lasm::OutputStream* stream);
    };

} // namespace Luac