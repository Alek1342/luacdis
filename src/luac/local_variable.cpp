#include "luac/local_variable.hpp"

Luac::LocalVariable::~LocalVariable() {
    if (this->m_varname) {
        delete this->m_varname;
    }
}

void Luac::LocalVariable::loadArray(Luac::InputStream* stream, Array<LocalVariable>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at locvars length");
    }
    i32 len = stream->get32();
    Array<LocalVariable>& locvars = *parr;
    locvars.init(len);
    if (len > 0) {
        for (size_t i = 0; i < len; i++) {
            locvars[i].m_varname->load(stream);
            if (!stream->request(8)) {
                LOGF("EOF at locvar pc");
            }
            locvars[i].m_startpc = stream->get32();
            locvars[i].m_endpc = stream->get32();
        }
    }
}

void Luac::LocalVariable::printArray(Lasm::OutputStream* stream, Array<LocalVariable>* parr) {
    i32 len = parr->length();
    LocalVariable* items = parr->items();
    if (len > 0) {
        for (size_t i = 0; i < len; i++) {
            items[i].print(stream);
        }
    } else if (len < 0) {
        stream->reserve(20);
        stream->write(".locvarcnt ");
        stream->write(len);
        stream->write("\n");
    }
}

void Luac::LocalVariable::print(Lasm::OutputStream* stream) {
    stream->reserve(stream->m_indent + 8);
    stream->write('\t', stream->m_indent);
    stream->write(".locvar ");
    m_varname->print(stream);
    stream->reserve(25);
    stream->write(" ");
    stream->write(m_startpc);
    stream->write(" ");
    stream->write(m_endpc);
    stream->write("\n");
}
