#include "luac/number.hpp"
#include "luac/header.hpp"
#include "luac/input_stream.hpp"
#include <climits>
#include <cmath>
#include <cstring>

// Detection that double is whole was reverse engineered so all bugs in it are intended
void Luac::Number::load(Luac::InputStream* stream) {
    if (stream->m_luacHeader != nullptr && stream->m_luacHeader->m_numberFormat == 1) {
        m_isInt = true;
        m_isWhole = true;
        m_inum = stream->get32();
    } else {
        m_isInt = false;
        m_isWhole = false;

        u64 doubleBits = stream->get64();
        memcpy(&m_fnum, &doubleBits, sizeof(u64));
        if ((LLONG_MAX & doubleBits) == 0) {
            m_isWhole = true;
        }
        int exponent = ((int)((doubleBits >> 52) & 2047)) - 1023;
        if (exponent >= 0 && exponent < 31) {
            long long fractionBits = doubleBits & DOUBLE_MANTISSA_MASK;
            int fractionShift = 52 - exponent;
            long long integerPrecisionMask = (1LL << fractionShift) - 1;
            if ((fractionBits & integerPrecisionMask) == 0) {
                m_isWhole = true;
            }
        }
    }
}

bool Luac::Number::isInt() {
    return m_isInt;
}

bool Luac::Number::isFloat() {
    return !m_isInt;
}

bool Luac::Number::isWholeFloat() {
    return !m_isInt && m_isWhole;
}

bool Luac::Number::isWhole() {
    return m_isWhole;
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
        } else if (isWholeFloat()) {
            stream->write(" ; integral");
        }
    }
}
