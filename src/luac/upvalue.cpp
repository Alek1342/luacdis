#include "luac/upvalue.hpp"

void Luac::Upvalue::loadArray(Luac::InputStream* stream, Array<Upvalue>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at upvalues length");
    }
    i32 len = stream->get32();
    parr->init(len);
    if (len > 0) {
        loadArray(stream, parr->items(), len);
    }
}

void Luac::Upvalue::loadArray(Luac::InputStream* stream, Upvalue* upvalues, size_t count) {
    size_t blen = static_cast<size_t>(count) * sizeof(Upvalue);
    stream->getBuffer(blen, upvalues);
}

void Luac::Upvalue::printArray(Lasm::OutputStream* stream, Array<Upvalue>* parr) {
    i32 len = parr->length();
    Upvalue* items = parr->items();
    if (len > 0) {
        for (size_t i = 0; i < (size_t)len; i++) {
            items[i].print(stream, i);
        }
    } else if (len < 0) {
        stream->reserve(20);
        stream->write(".upvaluecnt ");
        stream->write(len);
        stream->write("\n");
    }
}

void Luac::Upvalue::print(Lasm::OutputStream* stream, size_t index) {
    stream->reserve(stream->m_indent + 27);
    stream->write('\t', stream->m_indent);
    stream->write(".upvalue ");
    if (m_instack) {
        if (m_instack != 1) {
            stream->write("(");
            stream->writeHex(m_instack);
            stream->write(")");
        }
        stream->write("v");
    } else {
        stream->write("u");
    }
    stream->write(m_idx);
    stream->write(" ; u");
    stream->write((i32)index);
    stream->write("\n");
}