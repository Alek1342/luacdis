#include "luac/input_stream.hpp"
#include "bswap.hpp"
#include "log.hpp"
#include "luac/header.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

void Luac::InputStream::loadFromFile(char* path) {
    FILE* file = fopen(path, "rb");
    if (file == nullptr) {
        LOGF("fopen failed");
    }

    if (_fseeki64(file, 0, SEEK_END) != 0) {
        LOGF("seek fail");
    }

    size_t size = _ftelli64(file);

    if (size == 0) {
        fclose(file);
        LOGF("Empty file");
    }

    if (_fseeki64(file, 0, SEEK_SET) != 0) {
        LOGF("seek fail");
    }

    u8* buffer = static_cast<u8*>(malloc(size));

    if (buffer == nullptr) {
        fclose(file);
        LOGF("malloc failed buy more ram ig idk");
    }

    size_t bytesRead = fread(buffer, 1, size, file);
    fclose(file);

    if (bytesRead != size) {
        free(buffer);
        LOGF("fix disk ig idk");
    }

    m_start = buffer;
    m_cursor = buffer;
    m_end = buffer + size;
}

Luac::InputStream::~InputStream() {
    if (m_start) {
        free(m_start);
        m_start = m_cursor = m_end = nullptr;
    }
}

bool Luac::InputStream::request(size_t byteCount) const {
    return m_cursor + byteCount <= m_end;
}

u8 Luac::InputStream::getByte() {
    return *(m_cursor++);
}

u32 Luac::InputStream::get32() {
    u32* cur = reinterpret_cast<u32*>(m_cursor);
    u32 ret = *(cur++);
    m_cursor = reinterpret_cast<u8*>(cur);
    if (m_luacHeader != nullptr && m_luacHeader->m_isLittleEndian == 0) {
        ret = BSWAP32(ret);
    }
    return ret;
}

u64 Luac::InputStream::get64() {
    u64* cur = reinterpret_cast<u64*>(m_cursor);
    u64 ret = *(cur++);
    m_cursor = reinterpret_cast<u8*>(cur);
    if (m_luacHeader != nullptr && m_luacHeader->m_isLittleEndian == 0) {
        ret = BSWAP64(ret);
    }
    return ret;
}

void Luac::InputStream::getBuffer(size_t size, void* buffer) {
    if (size > 0) {
        if (m_cursor + size > m_end) {
            LOGF("EOF at Luac::InputStream::getBuffer");
        }
        memcpy(buffer, m_cursor, size);
        m_cursor += size;
    }
}