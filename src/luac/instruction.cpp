#include "luac/instruction.hpp"
#include "bswap.hpp"
#include "luac/header.hpp"

void Luac::Instruction::loadArray(Luac::InputStream* stream, Array<Instruction>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at code length");
    }
    i32 len = stream->get32();
    if (len < 0) {
        LOGF("Negative code len");
    }
    if (len > 0) {
        parr->init(len);
        loadArray(stream, parr->items(), len);
    }
}

void Luac::Instruction::loadArray(Luac::InputStream* stream, Instruction* instructions, size_t count) {
    if (count > 0) {
        // assumes Instruction = u32
        size_t blen = static_cast<size_t>(count) * sizeof(Instruction);
        u32* values = reinterpret_cast<u32*>(instructions);
        stream->getBuffer(blen, values);
        if (stream->m_luacHeader != nullptr && !stream->m_luacHeader->m_isLittleEndian) {
            for (size_t i = 0; i < count; ++i) {
                values[i] = BSWAP32(values[i]);
            }
        }
    }
}

void Luac::Instruction::printArray(Lasm::OutputStream* stream, Array<Instruction>* parr) {
    if (parr->length() > 0) {
        printArray(stream, parr->items(), parr->length());
    }
}

void Luac::Instruction::printArray(Lasm::OutputStream* stream, Instruction* instructions, size_t count) {
}