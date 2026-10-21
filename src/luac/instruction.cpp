#include "luac/instruction.hpp"
#include "bswap.hpp"
#include "lasm/output_stream.hpp"
#include "luac/header.hpp"

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
    for (size_t i = 0; i < count; ++i) {
        stream->reserve(50 + stream->m_indent);
        stream->write('\t', stream->m_indent);
        instructions[i].print(stream);
        stream->write("\n");
    }
}

void Luac::Instruction::printVKVK(Lasm::OutputStream* stream, int a, int b, int c) {
    stream->writeFast(a);
    if (b & 256) {
        b = b & 255;
        stream->write(" k");
    } else {
        stream->write(" v");
    }
    stream->writeFast(b);
    if (c & 256) {
        c = c & 255;
        stream->write(" k");
    } else {
        stream->write(" v");
    }
    stream->writeFast(c);
}

void Luac::Instruction::printGoto(Lasm::OutputStream* stream, size_t instCount, size_t pc, int a, int sbx) {
    stream->writeFast(a);
    stream->write(" ");
    if (instCount > 0) {
        int gotopos = pc + sbx + 1;
        if (gotopos >= instCount || gotopos < 0) {
            stream->write(sbx);
        } else {
            stream->write(":goto_");
            stream->write(gotopos);
        }
    } else {
        stream->write(sbx);
    }
}

void Luac::Instruction::print(Lasm::OutputStream* stream, size_t instCount, size_t pc) {
    int op = GET_OPCODE();
    int a = GETARG_A();
    int b = GETARG_B();
    int c = GETARG_C();
    switch (op) {
        case MOVE: {
            // MOVE vA vB
            // stack[A] = stack[B];
            stream->write("MOVE v");
            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c != 0) {
                stream->writeHex9(c);
            }
            break;
        }
        case LOADK: {
            // LOADK vA kBx
            // stack[A] = constants[Bx];
            int bx = GETARG_BX();
            stream->write("LOADK v");
            stream->writeFast(a);
            stream->write(" k");
            stream->write(bx);
            break;
        }
        case LOADKX: {
            // LOADKX vA
            // stack[A] = constants[EXTRAARG];
            // vm error if next instruction is not EXTRAARG
            stream->write("LOADKX v");
            stream->writeFast(a);
            if (b) {
                stream->write(" B = ");
                stream->writeHex9(b);
            }
            if (c) {
                stream->write(" C = ");
                stream->writeHex9(c);
            }
            break;
        }
        case LOADBOOL: {
            // LOADBOOL vA B C
            // stack[A] = (bool)B;
            // if (C) {
            //     pc = pc + 1;
            // }
            stream->write("LOADBOOL v");
            stream->writeFast(a);
            stream->write(" ");
            stream->writeFast(b);
            stream->write(" ");
            stream->writeFast(c);
            break;
        }
        case LOADNIL: {
            // LOADNIL vA..vA+B
            // for (int i = 0; i <= B; i++) {
            //     stack[A + i] = NIL;
            // }
            stream->write("LOADNIL v");
            stream->writeFast(a);
            stream->write("..v");
            stream->writeFast(a + b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case GETUPVAL: {
            // GETUPVAL vA uB
            // stack[A] = upvalues[B];
            stream->write("GETUPVAL v");
            stream->writeFast(a);
            stream->write(" u");
            stream->writeFast(b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case GETTABUP: {
            stream->write("GETTABUP v");
            stream->writeFast(a);
            stream->write(" u");
            stream->writeFast(b);

            if (c & 256) {
                c = c & 255;
                stream->write(" k");
            } else {
                stream->write(" v");
            }
            stream->writeFast(c);
            break;
        }
        case GETTABLE: {
            stream->write("GETTABLE v");

            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c & 256) {
                c = c & 255;
                stream->write(" k");
            } else {
                stream->write(" v");
            }
            stream->writeFast(c);
            break;
        }
        case SETTABUP: {
            stream->write("SETTABUP u");
            printVKVK(stream, a, b, c);
            break;
        }
        case SETUPVAL: {
            stream->write("SETUPVAL v");

            stream->writeFast(a);
            stream->write(" u");
            stream->writeFast(b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case SETTABLE: {
            stream->write("SETTABLE v");

            printVKVK(stream, a, b, c);
            break;
        }
        case NEWTABLE: {
            stream->write("NEWTABLE v");

            stream->writeFast(a);
            stream->write(" ");
            stream->writeFast(b);
            stream->write(" ");
            stream->writeFast(c);
            break;
        }
        case SELF: {
            stream->write("SELF v");

            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c & 256) {
                c = c & 255;
                stream->write(" k");
            } else {
                stream->write(" v");
            }
            stream->writeFast(c);
            break;
        }
        case ADD: {
            stream->write("ADD v");
            printVKVK(stream, a, b, c);
            break;
        }
        case SUB: {
            stream->write("SUB v");
            printVKVK(stream, a, b, c);
            break;
        }
        case MUL: {
            stream->write("MUL v");
            printVKVK(stream, a, b, c);
            break;
        }
        case DIV: {
            stream->write("DIV v");
            printVKVK(stream, a, b, c);
            break;
        }
        case MOD: {
            stream->write("MOD v");
            printVKVK(stream, a, b, c);
            break;
        }
        case POW: {
            stream->write("POW v");
            printVKVK(stream, a, b, c);
            break;
        }
        case UNM: {
            stream->write("UNM v");

            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case NOT: {
            stream->write("NOT v");

            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case LEN: {
            stream->write("LEN v");

            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case CONCAT: {
            stream->write("CONCAT v");

            stream->writeFast(a);
            stream->write(" v");
            if (c > b + 1) {
                stream->writeFast(b);
                stream->write("..v");
                stream->writeFast(c);
            } else {
                stream->writeFast(c - 1);
                stream->write("..v");
                stream->writeFast(c);
                if (b != c - 1) {
                    stream->writeHex9(b);
                }
            }
            break;
        }
        case JMP: {
            stream->write("JMP ");
            int sbx = GETARG_SBX();
            printGoto(stream, instCount, pc, a, sbx);
            break;
        }
        case EQ: {
            stream->write("EQ ");
            printVKVK(stream, a, b, c);
            break;
        }
        case LT: {
            stream->write("LT ");
            printVKVK(stream, a, b, c);
            break;
        }
        case LE: {
            stream->write("LE ");
            printVKVK(stream, a, b, c);
            break;
        }
        case TEST: {
            stream->write("TEST v");
            stream->writeFast(a);
            stream->write(" ");
            stream->writeFast(c);
            if (b) {
                stream->writeHex9(b);
            }
            break;
        }
        case TESTSET: {
            stream->write("TESTSET v");
            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            stream->write(" ");
            stream->writeFast(c);

            break;
        }
        case CALL: {
            stream->write("CALL v");
            stream->writeFast(a);
            if (b) {
                stream->write("..v");
                stream->writeFast(a + b - 1);
            }
            if (c) {
                if (c - 1) {
                    stream->write(" v");
                    stream->writeFast(a);
                    stream->write("..v");
                    stream->writeFast(a + c - 2);
                }
            } else {
                stream->write(" SET_TOP");
            }

            break;
        }
        case TAILCALL: {
            stream->write("TAILCALL v");
            stream->writeFast(a);
            if (b) {
                stream->write("..v");
                stream->writeFast(a + b - 1);
            }
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case RETURN: {
            stream->write("RETURN");
            if (b) {
                if (b != 1) {
                    stream->write(" v");
                    stream->writeFast(a);
                    stream->write("..v");
                    stream->writeFast(a + b - 2);
                } else { // b=1
                    if (a) {
                        stream->write(" A = ");
                        stream->writeHex((u8)a);
                    }
                }
            } else { // b=0
                stream->write(" v");
                stream->writeFast(a);
            }
            if (c) {
                stream->write(" C = ");
                stream->writeHex9(c);
            }
            break;
        }
        case FORLOOP: {
            stream->write("FORLOOP v");
            int sbx = GETARG_SBX();
            printGoto(stream, instCount, pc, a, sbx);
            break;
        }
        case FORPREP: {
            stream->write("FORPREP v");
            int sbx = GETARG_SBX();
            printGoto(stream, instCount, pc, a, sbx);
            break;
        }
        case TFORCALL: {
            stream->write("TFORCALL v");
            stream->writeFast(a);
            stream->write("..v");
            stream->writeFast(a + c + 2);
            if (b) {
                stream->writeHex9(b);
            }
            break;
        }
        case TFORLOOP: {
            stream->write("TFORLOOP v");
            int sbx = GETARG_SBX();
            printGoto(stream, instCount, pc, a, sbx);
            break;
        }
        case SETLIST: {
            stream->write("SETLIST v");
            stream->writeFast(a);
            if (b) {
                stream->write("..v");
                stream->writeFast(a + b);
            }
            stream->write(" ");
            stream->writeFast(c);
            break;
        }
        case CLOSURE: {
            stream->write("CLOSURE v");
            int bx = GETARG_BX();
            stream->writeFast(a);
            stream->write(" F");
            stream->write(bx);
            break;
        }
        case VARARG: {
            stream->write("VARARG ");
            if (b) {
                stream->write("v");
                stream->writeFast(a);
                stream->write("..v");
                stream->writeFast(a + b - 2);
            } else {
                stream->writeFast(a);
            }
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case EXTRAARG: {
            stream->write("EXTRAARG ");
            stream->write(GETARG_AX());
            break;
        }
        case IDIV: {
            stream->write("IDIV v");
            printVKVK(stream, a, b, c);
            break;
        }
        case BNOT: {
            stream->write("BNOT v");
            stream->writeFast(a);
            stream->write(" v");
            stream->writeFast(b);
            if (c) {
                stream->writeHex9(c);
            }
            break;
        }
        case BAND: {
            stream->write("BAND v");
            printVKVK(stream, a, b, c);
            break;
        }
        case BOR: {
            stream->write("BOR v");
            printVKVK(stream, a, b, c);
            break;
        }
        case BXOR: {
            stream->write("BXOR v");
            printVKVK(stream, a, b, c);
            break;
        }
        case SHL: {
            stream->write("SHL v");
            printVKVK(stream, a, b, c);
            break;
        }
        case SHR: {
            stream->write("SHR v");
            printVKVK(stream, a, b, c);
            break;
        }
        default: {
            stream->write(m_value);
            break;
        }
    }
}