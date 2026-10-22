#include "luac/constant.hpp"
#include "log.hpp"
#include "luac/array.hpp"
#include "luac/header.hpp"
#include "luac/input_stream.hpp"
#include "luac/number.hpp"
#include "luac/string.hpp"

Luac::Constant::~Constant() {
    switch (m_type) {
        case NIL:
            break;
        case BOOLEAN:
            delete static_cast<u8*>(m_value);
            break;
        case NUMBER:
            delete static_cast<Number*>(m_value);
            break;
        case STRING:
            delete static_cast<String*>(m_value);
            break;
        case INT:
            delete static_cast<i32*>(m_value);
            break;
        default:
            LOGF("Unknown constant type");
            break;
    }
}

void Luac::Constant::loadArray(Luac::InputStream* stream, Array<Constant>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at k length");
    }
    Array<Constant>& constants = *parr;
    i32 len = stream->get32();
    constants.init(len);
    if (len > 0) {
        loadArray(stream, constants.items(), len);
    }
}

void Luac::Constant::loadArray(Luac::InputStream* stream, Constant* constants, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (!stream->request(9)) {
            LOGF("EOF at k");
        }
        constants[i].load(stream);
    }
}

void Luac::Constant::load(Luac::InputStream* stream) {
    u8 type = stream->getByte();
    m_type = type;
    switch (type) {
        case Constant::NIL:
            // constant is initialized as nil
            break;
        case Constant::BOOLEAN: {
            m_value = new u8(stream->getByte());
            break;
        }
        case Constant::NUMBER: {
            Number* v = new Number;
            v->load(stream);
            m_value = v;
            break;
        }
        case Constant::STRING: {
            String* v = new String;
            v->load(stream);
            m_value = v;
            break;
        }
        case Constant::INT: {
            m_value = new i32(stream->get32());
            break;
        }
        default:
            LOGF("Unknown constant type");
    }
}

void Luac::Constant::printArray(Lasm::OutputStream* stream, Array<Constant>* parr) {
    i32 len = parr->length();
    Constant* items = parr->items();
    if (len > 0) {
        for (size_t i = 0; i < (size_t)len; i++) {
            items[i].print(stream, i);
        }
    } else if (len < 0) {
        stream->reserve(20);
        stream->write(".constcnt ");
        stream->write(len);
        stream->write("\n");
    }
}

void Luac::Constant::print(Lasm::OutputStream* stream, size_t index) {
    stream->reserve(stream->m_indent + 22);
    stream->write('\t', stream->m_indent);
    stream->write(".const ");
    switch (m_type) {
        case Constant::NIL:
            stream->write("nil");
            break;
        case Constant::BOOLEAN: {
            u8 v = *(static_cast<u8*>(m_value));
            if (v) {
                if (v != 1) {
                    stream->write("(");
                    stream->writeHex(v);
                    stream->write(")");
                }
                stream->write("true");
            } else {
                stream->write("false");
            }
            break;
        }
        case Constant::NUMBER:
            (static_cast<Number*>(m_value))->print(stream);
            break;
        case Constant::STRING:
            (static_cast<String*>(m_value))->print(stream);
            break;
        case Constant::INT: {
            i32 v = *(static_cast<i32*>(m_value));
            stream->write(v);
            stream->write("i");
            break;
        }
        default:
            LOGE("Uknown constant type");
    }
    stream->write(" ; k");
    stream->write((u32)index);
    stream->write("\n");
}