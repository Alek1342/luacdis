#include "luac/lineinfo.hpp"
#include "bswap.hpp"
#include "luac/header.hpp"

void Luac::Lineinfo::loadArray(Luac::InputStream* stream, Array<Lineinfo>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at lineinfo length");
    }
    i32 len = stream->get32();
    if (len < 0) {
        LOGF("Negative lineinfo len");
    }
    if (len > 0) {
        parr->init(len);
        // assumed Instruction = u32
        size_t blen = static_cast<size_t>(len) * sizeof(Lineinfo);
        u32* values = reinterpret_cast<u32*>(parr->items());
        stream->getBuffer(blen, values);
        if (stream->m_luacHeader != nullptr && !stream->m_luacHeader->m_isLittleEndian) {
            for (size_t i = 0; i < (size_t)len; i++) {
                values[i] = BSWAP32(values[i]);
            }
        }
    }
}

void Luac::Lineinfo::printArray(Lasm::OutputStream* stream, Array<Lineinfo>* parr) {
    i32 len = parr->length();
    Lineinfo* items = parr->items();
    if (len > 0) {
        for (size_t i = 0; i < (size_t)len; i++) {
            items[i].print(stream);
        }
    } else if (len < 0) {
        stream->reserve(20);
        stream->write(".lineinfocnt ");
        stream->write(len);
        stream->write(" ; shouldn't be negative\n");
    }
}

void Luac::Lineinfo::print(Lasm::OutputStream* stream) {
    stream->reserve(stream->m_indent + 22);
    stream->write('\t', stream->m_indent);
    stream->write(".lineinfo ");
    stream->write(m_value);
    stream->write("\n");
}