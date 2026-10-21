#include "luac/number.hpp"
#include "luac/header.hpp"
#include "luac/input_stream.hpp"
#include <climits>
#include <cmath>
#include <cstring>

// float to int detection part was ripped off reverse engineered code so all bugs in it are intended
void Luac::Number::load(Luac::InputStream* stream) {
    if (stream->m_luacHeader != nullptr && stream->m_luacHeader->m_numberFormat == 1) {
        i32 num = stream->get32();
        m_inum = num;
        if (num == FLOAT_INTEGER_SENTINEL) {
            m_fnum = INTEGER_TYPE_SENTINEL;
        }
    } else {
        u64 j = stream->get64();
        memcpy(&m_fnum, &j, sizeof(u64));
        if ((LLONG_MAX & j) == 0) {
            m_inum = FLOAT_INTEGER_SENTINEL;
        }
        int i = ((int)((j >> 52) & 2047)) - 1023;
        if (i >= 0 && i < 31) {
            long long j2 = 4503599627370495LL & j;
            int i2 = 52 - i;
            if ((((1LL << i2) - 1) & j2) == 0) {
                m_inum = FLOAT_INTEGER_SENTINEL;
            }
        }
    }
}

bool Luac::Number::isInt() {
    return (m_inum == FLOAT_INTEGER_SENTINEL && m_fnum == INTEGER_TYPE_SENTINEL) ||
           (m_fnum == DEFAULT_FLOAT && m_inum != FLOAT_INTEGER_SENTINEL);
}

bool Luac::Number::isFloat() {
    return (m_inum != FLOAT_INTEGER_SENTINEL || m_fnum != INTEGER_TYPE_SENTINEL) &&
           (m_fnum != DEFAULT_FLOAT || m_inum == FLOAT_INTEGER_SENTINEL);
}

bool Luac::Number::isFloatIntegral() {
    return (m_inum == FLOAT_INTEGER_SENTINEL && m_fnum != INTEGER_TYPE_SENTINEL);
}

bool Luac::Number::isTreatedAsInt() {
    return m_inum == FLOAT_INTEGER_SENTINEL || m_fnum == DEFAULT_FLOAT;
}

void Luac::Number::print(Lasm::OutputStream* stream) {
    stream->reserve(60);
    if (isInt()) {
        stream->write(m_inum);
    } else {
        stream->write(m_fnum);
        if (isnan(m_fnum)) {
            u64 bits;
            memcpy(&bits, &m_fnum, 8);
            uint64_t sign = bits >> 63;
            uint64_t frac = bits & ((UINT64_C(1) << 52) - 1);
            u64 id = sign * ((UINT64_C(1) << 52) - 1) + (frac - 1);
            if (id > 0) {
                stream->write("(");
                stream->writeHex56(id);
                stream->write(")");
            }
        } else if (isFloatIntegral()) {
            stream->write(" ; integral");
        }
    }
}
