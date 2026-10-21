#include "luac/tail.hpp"
#include "log.hpp"
#include <cstdio>
#include <cstdlib>

void Luac::Tail::init(size_t size) {
    if (size > 0) {
        void* bytes = malloc(size);
        if (bytes == nullptr) {
            LOGF("Failed to allocate buffer for tail");
        }
        m_bytes = (u8*)bytes;
        m_size = size;
    }
}

void Luac::Tail::load(Luac::InputStream* stream) {
    size_t size = stream->m_end - stream->m_cursor;
    if (size > 0) {
        void* bytes = malloc(size);
        if (bytes == nullptr) {
            LOGF("Failed to allocate buffer for tail");
        }
        stream->getBuffer(size, bytes);
        m_bytes = (u8*)bytes;
        m_size = size;
    }
}

Luac::Tail::~Tail() {
    if (m_bytes) {
        free(m_bytes);
    }
}

void Luac::Tail::dump(char* lasmPath) {
    if (!lasmPath) {
        LOGE("Luac::Tail::dump lasmPath = nullptr\n");
        return;
    }

    if (!m_bytes || m_size == 0) {
        LOGI("Luac::Tail::dump nothing to dump\n");
        return;
    }

    char path[4096];
    snprintf(path, sizeof(path), "%s.tail", lasmPath);

    FILE* file = fopen(path, "wb");
    if (!file) {
        LOGE("Luac::Tail::dump failed to open tail file\n");
        return;
    }

    fwrite(m_bytes, 1, m_size, file);
    fclose(file);
}