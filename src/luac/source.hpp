#pragma once

#include "lasm/output_stream.hpp"
#include "luac/string.hpp"
namespace Luac {
    class Source : public String {
    public:
        void print(Lasm::OutputStream* stream) override;
    };

} // namespace Luac