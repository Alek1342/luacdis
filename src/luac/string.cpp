#include "luac/string.hpp"
#include "lasm/output_stream.hpp"
#include "log.hpp"
#include "luac/header.hpp"
#include "luac/input_stream.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

size_t Luac::String::m_hugeCount = 0;

void Luac::String::init(u8* bytes, size_t size, u32 unused) {
    m_unused = unused;
    m_size = size;
    if (size > 0) {
        u8* buf = static_cast<u8*>(malloc(size));
        if (buf == nullptr) {
            LOGF("Luac::String.init malloc failed");
        }
        memcpy(buf, bytes, size);
        m_bytes = buf;
    }
}

Luac::String::~String() {
    if (m_bytes) {
        std::free(m_bytes);
    }
}

void Luac::String::load(Luac::InputStream* stream) {
    u64 sd;
    bool is64 = stream->m_luacHeader != nullptr && stream->m_luacHeader->m_sizeofSizeT == 8;
    if (is64) {
        if (!stream->request(8)) {
            LOGF("EOF at string length");
        }
        sd = stream->get64();
    } else {
        if (!stream->request(4)) {
            LOGF("EOF at string length");
        }
        sd = stream->get32();
    }
    i32 sz = sd & 0xFF'FF'FF'FF;
    if (sz < 0) {
        LOGF("Negative string size");
    }
    u8* buf = static_cast<u8*>(malloc(sz));
    if (buf == nullptr) {
        LOGF("Luac::String::load malloc failed");
    }
    stream->getBuffer(sz, buf);
    m_bytes = buf;
    m_size = sz;
    if (is64) {
        m_unused = sd >> 32;
    }
}

void Luac::String::print(Lasm::OutputStream* stream) {
    if (m_unused) {
        stream->reserve(12);
        stream->write("(");
        stream->writeHex(m_unused);
        stream->write(")");
    }
    if (m_size) {
        if (m_size > 3145728) {
            char path[512];
            snprintf(path, sizeof(path), "string_%zu.bin", m_hugeCount);
            FILE* f = fopen(path, "wb");
            if (f == nullptr) {
                LOGE("Huge string %zu lost", m_hugeCount);
            } else {
                fwrite(m_bytes, 1, m_size - 1, f);
                fclose(f);
            }
            stream->reserve(1024);
            stream->write("file(\"");
            stream->write(path, strlen(path));
            stream->write("\")");
            m_hugeCount++;
        } else {
            stream->reserve(m_size * 4 - 2);
            stream->write("\"");
            stream->writeEscape(m_bytes, m_size - 1);
            stream->write("\"");
        }
        if (m_bytes[m_size - 1] != 0x00) {
            stream->reserve(6);
            stream->write("(");
            stream->writeHex(m_bytes[m_size - 1]);
            stream->write(")");
        }
    } else {
        stream->reserve(4);
        stream->write("null");
    }
}