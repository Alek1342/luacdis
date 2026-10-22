#include "luac/upvaluename.hpp"

void Luac::Upvaluename::loadArray(Luac::InputStream* stream, Array<Upvaluename>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at upvals length");
    }
    i32 len = stream->get32();
    Array<Upvaluename>& upvalnames = *parr;
    upvalnames.init(len);
    if (len > 0) {
        for (size_t i = 0; i < (size_t)len; i++) {
            upvalnames[i].load(stream);
        }
    }
}

void Luac::Upvaluename::printArray(Lasm::OutputStream* stream, Array<Upvaluename>* parr) {
    i32 len = parr->length();
    Upvaluename* items = parr->items();
    if (len > 0) {
        for (size_t i = 0; i < (size_t)len; i++) {
            items[i].print(stream);
        }
    } else if (len < 0) {
        stream->reserve(20);
        stream->write(".upvnamecnt ");
        stream->write(len);
        stream->write("\n");
    }
}

void Luac::Upvaluename::print(Lasm::OutputStream* stream) {
    stream->reserve(stream->m_indent + 8);
    stream->write('\t', stream->m_indent);
    stream->write(".upvname ");
    String::print(stream);
    stream->reserve(1);
    stream->write("\n");
}
