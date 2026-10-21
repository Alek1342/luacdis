#pragma once
#include "lasm/output_stream.hpp"
#include "types.hpp"

namespace Luac {
    class InputStream;
    struct Header {
    public:
        static constexpr u8 expectedSignature[4] = {0x1B, 'L', 'u', 'a'};
        static constexpr u8 expectedTail[6] = {0x19, 0x93, 0x0D, 0x0A, 0x1A, 0x0A};
        static constexpr u8 allowedNumberFormats[3] = {0x00, 0x01, 0x04};
        static constexpr size_t allowedNumberFormatsLength = sizeof(allowedNumberFormats);
        static constexpr size_t signatureSize = sizeof(expectedSignature);
        static constexpr size_t tailSize = sizeof(expectedTail);
        u8 m_signature[signatureSize];
        u8 m_version;
        u8 m_format;
        u8 m_isLittleEndian;
        u8 m_sizeofInt;
        u8 m_sizeofSizeT;
        u8 m_sizeofInstruction;
        u8 m_sizeofLuaNumber;
        u8 m_numberFormat;
        u8 m_tail[tailSize];
        void load(Luac::InputStream* stream);
        void print(Lasm::OutputStream* stream);
    };
} // namespace Luac