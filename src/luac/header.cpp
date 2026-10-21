#include "luac/header.hpp"
#include "lasm/output_stream.hpp"
#include "log.hpp"
#include "luac/input_stream.hpp"
#include <cstring>

void Luac::Header::load(Luac::InputStream* stream) {
    stream->m_luacHeader = this;

    if (!stream->request(sizeof(Header))) {
        LOGF("EOF at header");
    }

    stream->getBuffer(Header::signatureSize, this->m_signature);

    if (memcmp(this->m_signature, Header::expectedSignature, Header::signatureSize) != 0) {
        LOGF("Invalid signature");
    }

    this->m_version = stream->getByte();
    this->m_format = stream->getByte();
    this->m_isLittleEndian = stream->getByte();
    this->m_sizeofInt = stream->getByte();
    this->m_sizeofSizeT = stream->getByte();
    this->m_sizeofInstruction = stream->getByte();
    this->m_sizeofLuaNumber = stream->getByte();

    u8 nf = stream->getByte();
    bool nfisvalid = false;
    for (size_t i = 0; i < Header::allowedNumberFormatsLength; i++) {
        if (nf == Header::allowedNumberFormats[i]) {
            nfisvalid = true;
            break;
        }
    }
    if (!nfisvalid) {
        LOGF("Invalid number format");
    }
    this->m_numberFormat = nf;

    stream->getBuffer(Header::tailSize, this->m_tail);

    if (memcmp(this->m_tail, Header::expectedTail, Header::tailSize) != 0) {
        LOGF("Invalid header tail");
    }
}

void Luac::Header::print(Lasm::OutputStream* stream) {
    stream->reserve(1024);

    stream->write(".version ");
    stream->writeHex(m_version);
    stream->write("\n");

    stream->write(".format ");
    stream->writeHex(m_format);
    stream->write("\n");

    stream->write(".isLitteEndian ");
    stream->writeHex(m_isLittleEndian);
    stream->write("\n");

    stream->write(".sizeOfInt ");
    stream->writeHex(m_sizeofInt);
    stream->write("\n");

    stream->write(".sizeOfSizeT ");
    stream->writeHex(m_sizeofSizeT);
    stream->write("\n");

    stream->write(".sizeOfInstruction ");
    stream->writeHex(m_sizeofInstruction);
    stream->write("\n");

    stream->write(".sizeOfLuaNumber ");
    stream->writeHex(m_sizeofLuaNumber);
    stream->write("\n");

    stream->write(".numberFormat ");
    stream->writeHex(m_numberFormat);
    stream->write("\n");
}