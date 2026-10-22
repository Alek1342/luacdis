#include "luac/instruction.hpp"
#include "bswap.hpp"
#include "lasm/output_stream.hpp"
#include "log.hpp"
#include "luac/header.hpp"
#include <algorithm>
#include <cstdlib>
#include <vector>

void Luac::Instruction::loadArray(Luac::InputStream* stream, Array<Instruction>* parr) {
    if (!stream->request(4)) {
        LOGF("EOF at code length");
    }
    i32 len = stream->get32();
    if (len < 0) {
        LOGF("Negative code len");
    }
    if (len > 0) {
        parr->init(len);
        loadArray(stream, parr->items(), len);
    }
}

void Luac::Instruction::loadArray(Luac::InputStream* stream, Instruction* instructions, size_t count) {
    if (count > 0) {
        // assumes Instruction = u32
        size_t blen = static_cast<size_t>(count) * sizeof(Instruction);
        u32* values = reinterpret_cast<u32*>(instructions);
        stream->getBuffer(blen, values);
        if (stream->m_luacHeader != nullptr && !stream->m_luacHeader->m_isLittleEndian) {
            for (size_t i = 0; i < count; ++i) {
                values[i] = BSWAP32(values[i]);
            }
        }
    }
}

void Luac::Instruction::printArray(Lasm::OutputStream* stream, Array<Instruction>* parr) {
    i32 len = parr->length();
    if (len > 0) {
        printArray(stream, parr->items(), len);
    } else if (len < 0) {
        stream->reserve(20);
        stream->write(".instcnt ");
        stream->write(len);
        stream->write("\n");
    }
}

void Luac::Instruction::printArray(Lasm::OutputStream* stream, Instruction* instructions, size_t count) {
    std::vector<size_t> labels;
    int lb = 0;
    for (size_t i = 0; i < count; ++i) {
        // JMP FORLOOP FORPREP TFORLOOP
        Instruction inst = instructions[i];
        switch (inst.GET_OPCODE()) {
            case JMP:
            case FORLOOP:
            case FORPREP:
            case TFORLOOP:
                lb = i + inst.GETARG_SBX() + 1;
                if (lb >= count || lb < 0) {
                    break;
                }
                labels.push_back(lb);
                break;
            default:
                break;
        }
    }
    std::sort(labels.begin(), labels.end());
    labels.erase(std::unique(labels.begin(), labels.end()), labels.end());
    size_t labels_len = labels.size();
    size_t label_i = 1;
    size_t label = 0;
    if (labels_len > 0) {
        label = labels[0];
    }
    for (size_t i = 0; i < count; ++i) {
        if (labels_len && i == label) {
            stream->reserve(30 + stream->m_indent);
            stream->write('\t', stream->m_indent);
            stream->write(":goto_");
            stream->write((u32)label);
            stream->write("\n");
            if (label_i < labels_len) {
                label = labels.at(label_i++);
            }
        }
        stream->reserve(50 + stream->m_indent);
        stream->write('\t', stream->m_indent);
        instructions[i].print(stream, count, i);
        stream->write("\n");
    }
}

void Luac::Instruction::printVKIVKI(Lasm::OutputStream* stream, int arg1, int arg2) {
    printVKI(stream, arg1);
    printVKI(stream, arg2);
}

void Luac::Instruction::printVITVI(Lasm::OutputStream* stream, int arg1, int arg2) {
    stream->write(" v");
    stream->writeFast(arg1);
    stream->write("..v");
    stream->writeFast(arg2);
}

void Luac::Instruction::printVKI(Lasm::OutputStream* stream, int arg) {
    if (arg & 256) {
        arg = arg & 255;
        stream->write(" k");
    } else {
        stream->write(" v");
    }
    stream->writeFast(arg);
}

void Luac::Instruction::printVI(Lasm::OutputStream* stream, int arg) {
    stream->write(" v");
    stream->writeFast(arg);
}

void Luac::Instruction::printTVI(Lasm::OutputStream* stream, int arg) {
    stream->write("..v");
    stream->writeFast(arg);
}

void Luac::Instruction::printI(Lasm::OutputStream* stream, int arg) {
    stream->write(" ");
    stream->writeFast(arg);
}

void Luac::Instruction::printUI(Lasm::OutputStream* stream, int arg) {
    stream->write(" u");
    stream->writeFast(arg);
}

void Luac::Instruction::printKIF(Lasm::OutputStream* stream, int arg) {
    stream->write(" k");
    stream->writeFast(arg);
}

void Luac::Instruction::printKI(Lasm::OutputStream* stream, int arg) {
    stream->write(" k");
    stream->write(arg);
}

void Luac::Instruction::printVIVKIVKI(Lasm::OutputStream* stream, int arg1, int arg2, int arg3) {
    printVI(stream, arg1);
    printVKI(stream, arg2);
    printVKI(stream, arg3);
}

void Luac::Instruction::printVIVI(Lasm::OutputStream* stream, int arg1, int arg2) {
    printVI(stream, arg1);
    printVI(stream, arg2);
}

void Luac::Instruction::printGoto(Lasm::OutputStream* stream, size_t instCount, size_t pc, int arg) {
    stream->write(" ");
    if (instCount > 0) {
        int gotopos = pc + arg + 1;
        if (gotopos >= instCount || gotopos < 0) {
            stream->write(arg);
        } else {
            stream->write(":goto_");
            stream->write(gotopos);
        }
    } else {
        stream->write(arg);
    }
}

void Luac::Instruction::print(Lasm::OutputStream* s, size_t instCount, size_t pc) {
    int op = GET_OPCODE();
    int a = GETARG_A();
    int b = GETARG_B();
    int c = GETARG_C();
    uint unused = 0;
    switch (op) {
        case MOVE: {
            // MOVE vA vB
            // stack[A] = stack[B];
            s->write("MOVE");
            printVIVI(s, a, b);
            unused = c;
            break;
        }
        case LOADK: {
            // LOADK vA kBx
            // stack[A] = constants[Bx];
            s->write("LOADK");
            printVI(s, a);
            printKI(s, GETARG_BX());
            break;
        }
        case LOADKX: {
            // LOADKX vA
            // stack[A] = constants[EXTRAARG];
            // vm error if next instruction is not EXTRAARG
            s->write("LOADKX");
            printVI(s, a);
            unused = GETARG_BX();
            break;
        }
        case LOADBOOL: {
            // LOADBOOL vA B C
            // stack[A] = (bool)B;
            // if (C) {
            //     pc = pc + 1;
            // }
            s->write("LOADBOOL");
            printVI(s, a);
            printI(s, b);
            printI(s, c);
            break;
        }
        case LOADNIL: {
            // LOADNIL vA..vA+B
            // for (int i = 0; i <= B; i++) {
            //     stack[A + i] = NIL;
            // }
            s->write("LOADNIL");
            printVITVI(s, a, a + b);
            unused = c;
            break;
        }
        case GETUPVAL: {
            // GETUPVAL vA uB
            // stack[A] = upvalues[B];
            s->write("GETUPVAL");
            printVI(s, a);
            printUI(s, b);
            unused = c;
            break;
        }
        case GETTABUP: {
            s->write("GETTABUP");
            printVI(s, a);
            printUI(s, b);
            printVKI(s, c);
            break;
        }
        case GETTABLE: {
            s->write("GETTABLE");
            printVIVI(s, a, b);
            printVKI(s, c);
            break;
        }
        case SETTABUP: {
            s->write("SETTABUP");
            printUI(s, a);
            printVKI(s, b);
            printVKI(s, c);

            break;
        }
        case SETUPVAL: {
            s->write("SETUPVAL");
            printVI(s, a);
            printUI(s, b);
            unused = c;
            break;
        }
        case SETTABLE: {
            s->write("SETTABLE");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case NEWTABLE: {
            s->write("NEWTABLE");
            printVI(s, a);
            printI(s, b);
            printI(s, c);
            break;
        }
        case SELF: {
            s->write("SELF");
            printVIVI(s, a, b);
            printVKI(s, c);
            break;
        }
        case ADD: {
            s->write("ADD");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case SUB: {
            s->write("SUB");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case MUL: {
            s->write("MUL");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case DIV: {
            s->write("DIV");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case MOD: {
            s->write("MOD");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case POW: {
            s->write("POW");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case UNM: {
            s->write("UNM");
            printVIVI(s, a, b);
            unused = c;
            break;
        }
        case NOT: {
            s->write("NOT");
            printVIVI(s, a, b);
            unused = c;
            break;
        }
        case LEN: {
            s->write("LEN");
            printVIVI(s, a, b);
            unused = c;
            break;
        }
        case CONCAT: {
            s->write("CONCAT");
            printVI(s, a);

            if (b + 1 <= c) {
                printVITVI(s, b, c);
            } else {
                printVITVI(s, c - 1, c);
                unused = b + 1 - c;
            }
            break;
        }
        case JMP: {
            s->write("JMP");
            printI(s, a);
            printGoto(s, instCount, pc, GETARG_SBX());
            break;
        }
        case EQ: {
            s->write("EQ");
            printI(s, a);
            printVKI(s, b);
            printVKI(s, c);
            break;
        }
        case LT: {
            s->write("LT");
            printI(s, a);
            printVKI(s, b);
            printVKI(s, c);
            break;
        }
        case LE: {
            s->write("LE");
            printI(s, a);
            printVKI(s, b);
            printVKI(s, c);
            break;
        }
        case TEST: {
            s->write("TEST");
            printVI(s, a);
            printI(s, c);
            unused = b;
            break;
        }
        case TESTSET: {
            s->write("TESTSET");
            printVIVI(s, a, b);
            printI(s, c);
            break;
        }
        case CALL: {
            s->write("CALL");
            printVI(s, a);
            if (b) {
                printTVI(s, a + b - 1);
            }
            if (c) {
                if (c - 1) {
                    printVI(s, a);
                    printTVI(s, a + c - 2);
                }
            } else {
                s->write(" SET_TOP");
            }
            break;
        }
        case TAILCALL: {
            s->write("TAILCALL");
            printVI(s, a);
            if (b) {
                printTVI(s, a + b - 1);
            }
            unused = c;
            break;
        }
        case RETURN: {
            s->write("RETURN");
            if (!b || b > 1) {
                printVI(s, a);
                if (b > 1) {
                    printTVI(s, a + b - 2);
                }
            } else if (a) {
                unused = a;
            }
            if (c) {
                unused |= (c << 8);
            }
            break;
        }
        case FORLOOP: {
            s->write("FORLOOP");
            printVI(s, a);
            printGoto(s, instCount, pc, GETARG_SBX());
            break;
        }
        case FORPREP: {
            s->write("FORPREP");
            printVI(s, a);
            printGoto(s, instCount, pc, GETARG_SBX());
            break;
        }
        case TFORCALL: {
            s->write("TFORCALL");
            printVI(s, a);
            printTVI(s, a + c + 2);
            unused = b;
            break;
        }
        case TFORLOOP: {
            s->write("TFORLOOP");
            printVI(s, a);
            printGoto(s, instCount, pc, GETARG_SBX());
            break;
        }
        case SETLIST: {
            s->write("SETLIST");
            printVI(s, a);
            if (b) {
                printTVI(s, a + b);
            }
            printI(s, c);
            break;
        }
        case CLOSURE: {
            s->write("CLOSURE");
            printVI(s, a);
            s->write(" F");
            s->write(GETARG_BX());
            break;
        }
        case VARARG: {
            s->write("VARARG");
            if (b) {
                printVI(s, a);
                printTVI(s, a + b - 2);
            } else {
                printI(s, a);
            }
            unused = c;
            break;
        }
        case EXTRAARG: {
            s->write("EXTRAARG ");
            s->write(GETARG_AX());
            break;
        }
        case IDIV: {
            s->write("IDIV");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case BNOT: {
            s->write("BNOT");
            printVIVI(s, a, b);
            unused = c;
            break;
        }
        case BAND: {
            s->write("BAND");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case BOR: {
            s->write("BOR");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case BXOR: {
            s->write("BXOR");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case SHL: {
            s->write("SHL");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        case SHR: {
            s->write("SHR");
            printVIVKIVKI(s, a, b, c);
            break;
        }
        default: {
            s->write(m_value);
            break;
        }
    }
    if (unused) {
        s->writeHex(unused);
    }
}