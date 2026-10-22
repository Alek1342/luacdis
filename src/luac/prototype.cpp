
#include "luac/prototype.hpp"
#include "lasm/output_stream.hpp"
#include "lasm/print_state.hpp"
#include "luac/header.hpp"
#include "luac/input_stream.hpp"
#include "luac/source.hpp"
#include "stack.hpp"
#include <cstddef>

void Luac::Prototype::load(Luac::InputStream* stream) {
    loadArray(stream, this, 1);
}

void Luac::Prototype::loadArray(Luac::InputStream* stream, Array<Prototype>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at p length");
    }
    i32 len = stream->get32();
    parr->init(len);
    if (len > 0) {
        loadArray(stream, parr->items(), parr->length());
    }
}

void Luac::Prototype::loadArray(Luac::InputStream* stream, Prototype* p, size_t count) {
    Stack<Prototype*> ps;
    Stack<size_t> cnts;
    bool ispstart = true;
    while (true) {
        if (ispstart) {
            p->loadPHeader(stream);
            p->m_code.load(stream);
            p->m_constants.load(stream);
            if (!stream->request(4)) {
                LOGF("EOF at p length");
            }
            i32 len = stream->get32();
            p->m_functions.init(len);
            if (len > 0) {
                ps.push(p);
                cnts.push(count);
                p = p->m_functions.items();
                count = len;
            } else {
                ispstart = false;
            }
        } else {
            p->m_upvalues.load(stream);
            p->m_source.load(stream);
            p->m_lineinfo.load(stream);
            p->m_locvars.load(stream);
            p->m_upvalnames.load(stream);
            count--;
            if (count == 0) {
                if (ps.length() == 0) {
                    break;
                } else {
                    p = ps.pop();
                    count = cnts.pop();
                }
            } else {
                p++;
                ispstart = true;
            }
        }
    }
}

void Luac::Prototype::loadPHeader(Luac::InputStream* stream) {
    if (!stream->request(12)) {
        LOGF("EOF at prototype header");
    }
    m_linedefined = stream->get32();
    m_lastlinedefined = stream->get32();
    m_numparams = stream->getByte();
    m_is_vararg = stream->getByte();
    m_maxstacksize = stream->getByte();
}

void Luac::Prototype::print(Lasm::OutputStream* stream) {
    Lasm::PrintState ps;
    stream->m_printState = &ps;
    printArray(stream, this, 1);
}

void Luac::Prototype::printArray(Lasm::OutputStream* stream, Array<Prototype>* parr) {
    if (stream->m_printState == nullptr) {
        Lasm::PrintState ps;
        stream->m_printState = &ps;
    }
    i32 len = parr->length();
    if (len < 0) {
        stream->reserve(stream->m_indent + 30);
        stream->write('\t', stream->m_indent);
        stream->write(".fcount ");
        stream->write(len);
        stream->write("\n");
        return;
    }
    if (len > 0) {
        printArray(stream, parr->items(), len);
    }
}

void Luac::Prototype::printArray(Lasm::OutputStream* stream, Prototype* p, size_t count) {
    size_t fnum = 0;
    Stack<Prototype*> ps;
    Stack<size_t> cnts;
    Stack<size_t> fnums;

    bool ispstart = true;
    while (true) {
        if (ispstart) {
            p->printPHeader(stream, fnum);
            p->m_source.print(stream);
            p->m_lineinfo.print(stream);
            p->m_locvars.print(stream);
            p->m_upvalues.print(stream);
            p->m_upvalnames.print(stream);
            p->m_constants.print(stream);
            p->m_code.print(stream);
            i32 len = p->m_functions.length();
            if (len > 0) {
                ps.push(p);
                cnts.push(count);
                fnums.push(fnum);
                p = p->m_functions.items();
                count = len;
                fnum = 0;
            } else {
                if (len < 0) {
                    stream->reserve(stream->m_indent + 30);
                    stream->write('\t', stream->m_indent);
                    stream->write(".fcount ");
                    stream->write(len);
                    stream->write("\n");
                }
                ispstart = false;
            }
        } else {
            stream->m_indent--;
            stream->reserve(stream->m_indent + 50);
            stream->write('\t', stream->m_indent);
            stream->write(".end ; F");
            stream->write((i32)fnum);
            stream->write("\n");
            count--;
            fnum++;
            if (count == 0) {
                if (ps.length() == 0) {
                    break;
                } else {
                    p = ps.pop();
                    count = cnts.pop();
                    fnum = fnums.pop();
                }
            } else {
                p++;
                ispstart = true;
            }
        }
    }
}

void Luac::Prototype::printPHeader(Lasm::OutputStream* stream, size_t fnum) {
    stream->reserve(stream->m_indent * 5 + 128);
    stream->write('\t', stream->m_indent);
    stream->write(".func ; F");
    stream->write((i32)fnum);
    stream->write("\n");
    stream->m_indent++;

    stream->write('\t', stream->m_indent);
    stream->write(".linedefined ");
    stream->write(m_linedefined);
    stream->write("\n");

    stream->write('\t', stream->m_indent);
    stream->write(".lastlinedefined ");
    stream->write(m_lastlinedefined);
    stream->write("\n");

    stream->write('\t', stream->m_indent);
    stream->write(".numparams ");
    stream->write(m_numparams);
    stream->write("\n");

    stream->write('\t', stream->m_indent);
    stream->write(".is_vararg ");
    stream->write(m_is_vararg);
    stream->write("\n");

    stream->write('\t', stream->m_indent);
    stream->write(".maxstacksize ");
    stream->write(m_maxstacksize);
    stream->write("\n");
}