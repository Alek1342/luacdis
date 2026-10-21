#pragma once
#include "lasm/output_stream.hpp"
#include "luac/header.hpp"
#include "luac/input_stream.hpp"
#include "luac/prototype.hpp"
#include "luac/tail.hpp"

namespace Luac {

    class File {
    public:
        Header m_header;
        Prototype m_prototype;
        Tail m_tail;
        void loadFromFile(char* path);
        void load(Luac::InputStream* stream);
        void printToFile(char* path);
        void print(Lasm::OutputStream* stream);
        void addLasmComment(Lasm::OutputStream* stream);
    };
} // namespace Luac