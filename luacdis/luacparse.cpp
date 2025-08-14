#include "luacparse.hpp"


unsigned char readbyte(unsigned char*& luacpos) {
    return *luacpos++;
}

int readintlittle(unsigned char*& luacpos) {
    int ret = *(int*)luacpos;
    luacpos += 4;
    return ret;
}

unsigned int readuintlittle(unsigned char*& luacpos) {
    unsigned int ret = *(unsigned int*)luacpos;
    luacpos += 4;
    return ret;
}

int readintbig(unsigned char*& luacpos) {
    int ret = (luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3];
    luacpos += 4;
    return ret;
}

unsigned int readuintbig(unsigned char*& luacpos) {
    unsigned int ret = (luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3];
    luacpos += 4;
    return ret;
}

void readbytesluastring(unsigned char*& luacpos, int n) {
    unsigned char tmp;;
    for (int i = 0; i < n; i++) {
        tmp = luacpos[i];
        writestr(luaencodechr[tmp], luaencodesz[tmp]);
    }
    luacpos += n;
}

int readsize4little(unsigned char*& luacpos) {
    int ret = *(int*)luacpos;
    luacpos += 4;
    return ret;
}

int readsize4big(unsigned char*& luacpos) {
    int ret = (luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3];
    luacpos += 4;
    return ret;
}

int readsize8little(unsigned char*& luacpos) {
    int ret = *(int*)luacpos;
    luacpos += 4;
    int tmp = *(int*)luacpos;
    luacpos += 4;
    if (tmp != 0) {
        writehexintadditional(tmp);
    }
    return ret;
}

int readsize8big(unsigned char*& luacpos) {
    int ret = (luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3];
    luacpos += 4;
    int tmp = (luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3];
    luacpos += 4;
    if (tmp != 0) {
        writehexintadditional(tmp);
    }
    return ret;
}

void readluanumibig(unsigned char*& luacpos) {
    writeint((luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3]);
    luacpos += 4;
}

void readluanumilittle(unsigned char*& luacpos) {
    writeint(*(int*)luacpos);
    luacpos += 4;
}

void readluaintbig(unsigned char*& luacpos) {
    writeint((luacpos[0] << 24) | (luacpos[1] << 16) | (luacpos[2] << 8) | luacpos[3]);
    writestr("i");
    luacpos += 4;
}

void readluaintlittle(unsigned char*& luacpos) {
    writeint(*(int*)luacpos);
    writestr("i");
    luacpos += 4;
}

void readluanumdlittle(unsigned char*& luacpos) {
    long long bits = *(long long*)luacpos;
    luacpos += 8;
    if ((LLONG_MAX & bits) == 0) {
        writestr("0");
        return;
    }
    int e = ((int)((bits >> 52) & 2047)) - 1023;
    if (e >= 0 && e < 31) {
        long long f = 4503599627370495L & bits;
        int shift = 52 - e;
        if ((((1 << shift) - 1) & f) == 0) {
            int intValue = (1 << e) | ((int)(f >> shift));
            if ((bits >> 63) != 0) {
                intValue = -intValue;
            }
            writeint(intValue);
            return;
        }
    }
    writefloat(*(double*)&bits);
    return;
}

void readluanumdbig(unsigned char*& luacpos) {
    long long bits = ((long long)luacpos[0] << 56) | ((long long)luacpos[1] << 48) | ((long long)luacpos[2] << 40) | ((long long)luacpos[3] << 32) | (luacpos[4] << 24) | (luacpos[5] << 16) | (luacpos[6] << 8) | luacpos[7];
    luacpos += 8;
    if ((LLONG_MAX & bits) == 0) {
        writestr("0");
        return;
    }
    int e = ((int)((bits >> 52) & 2047)) - 1023;
    if (e >= 0 && e < 31) {
        long long f = 4503599627370495L & bits;
        int shift = 52 - e;
        if ((((1 << shift) - 1) & f) == 0) {
            int intValue = (1 << e) | ((int)(f >> shift));
            if ((bits >> 63) != 0) {
                intValue = -intValue;
            }
            writeint(intValue);
            return;
        }
    }
    writefloat(*(double*)&bits);
    return;
}



void disasm(unsigned int inst, int instcnt, int pc) {
    switch(inst & 0x3F) {
    case 0: {
        //MOVE vA vB permut -> vA = vB
        writestr("MOVE v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 1: {
        //LOADK vA kB -> vA = kB
        writestr("LOADK v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 14;
        writeint(a);
        writestr(" k");
        writeint(b);
        break;
    }
    case 2: {
        //LOADKX vA k[EXRAARG] -> vA = k[EXRAARG] pc++
        //crash if there is no EXRAARG
        writestr("LOADKX v");
        int a = (inst >> 6) & 0xFF;
        int unused = inst >> 14;
        writeint(a);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 3: {
        //               B       C
        //LOADBOOL vA [0;511] [0;511] -> vA = (bool)B if (C) pc++
        writestr("LOADBOOL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" ");
        writeint(b);
        writestr(" ");
        writeint(c);
        break;
    }
    case 4: {
        //LOADNIL vA..vA+B  [0;511]   -> vA,v[A+1],..v[A+B] = nil
        writestr("LOADNIL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        if (b != 0) {
            writestr("..v");
            writeint(a+b);
        }
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 5: {
        //GETUPVAL vA uB -> vA = uB
        writestr("GETUPVAL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        writestr(" u");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 6: {
        //GETTABUP vA uB vk(C) -> vA = uB[vk(C)]
        writestr("GETTABUP v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" u");
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 7: {
        //GETTABLE vA vB vk(C) -> vA = vB[vk(C)]
        writestr("GETTABLE v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 8: {
        writestr("SETTABUP u");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 9: {
        writestr("SETUPVAL u");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(b);
        writestr(" v");
        writeint(a);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 10: {
        writestr("SETTABLE v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 11: {
        writestr("NEWTABLE v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23; //aarye
        int c = (inst >> 14) & 511; //hash
        writeint(a);
        writestr(" ");
        writeint(b);
        writestr(" ");
        writeint(c);
        break;
    }
    case 12: {
        writestr("SELF v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 13: {
        writestr("ADD v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 14: {
        writestr("SUB v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 15: {
        writestr("MUL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 16: {
        writestr("DIV v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 17: {
        writestr("MOD v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 18: {
        writestr("POW v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 19: {
        writestr("UNM v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 20: {
        writestr("NOT v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 21: {
        writestr("LEN v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 22: {
        writestr("CONCAT v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        if (c > b + 1) {
            writeint(b);
            writestr("..v");
            writeint(c);
        } else {
            writeint(c - 1);
            writestr("..v");
            writeint(c);
            writestr(" ");
            writeint(b);
        }
        break;
    }
    case 23: {
        writestr("JMP ");
        int a = (inst >> 6) & 0xFF;
        int b = (inst >> 14) - 0x1ffff;
        writeint(a);
        writestr(" ");
        int gotopos = pc + b + 1;
        if (gotopos >= instcnt || gotopos < 0) {
            writeint(b);
        } else {
            writestr(":goto_");
            writeint(gotopos);
        }
        break;
    }
    case 24: {
        writestr("EQ ");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 25: {
        writestr("LT ");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 26: {
        writestr("LE ");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 27: {
        writestr("TEST v");
        int a = (inst >> 6) & 0xFF;
        int b = (inst >> 14) & 511;
        int unused = inst >> 23;
        writeint(a);
        writestr(" ");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 28: {
        writestr("TESTSET v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        writestr(" ");
        writeint(c);
        break;
    }
    case 29: {
        //c = 0 -> varargs
        //c = 1 -> NONE
        //c > 1 -> c-1 retvals
        //b -1  args

        // vA+1 .. vA+B-1 arg range
        writestr("CALL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b) {
            if (b - 1) {
                writestr(" v");
                writeint(a + 1);
                writestr("..v");
                writeint(a + b - 1);
            } else {
                writestr(" NO_ARGS");
            }
        } else {
            writestr(" VARARGS");
        }
        if (c) {
            if (c - 1) {
                writestr(" v");
                writeint(a);
                writestr("..v");
                writeint(a + c - 2);
            } else{
                writestr(" NO_RETVALS");
            }
        } else{
            writestr(" SET_VARARG");
        }
        break;
    }
    case 30: {
        writestr("TAILCALL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        if (b) {
            if (b - 1) {
                if (b - 2) {
                    writestr(" v");
                    writeint(a + 1);
                    writestr("..v");
                    writeint(a + b-1);
                } else {
                    writestr(" v");
                    writeint(a + 1);
                }
            } 
        } else {
            writestr(" GET_TOP");
        }
        if (unused) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 31: {
        writestr("RETURN");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        switch (b) {
        case 0: 
            writestr(" ");
            writeint(a);
            writestr(" GET_TOP");
            break;
        case 1:
            break;
        case 2:
            writestr(" v");
            writeint(a);
            break;
        default: 
            writestr(" v");
            writeint(a);
            writestr("..v");
            writeint(a + b - 2);
            break;
        }
        if (unused) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 32: {
        writestr("FORLOOP v");
        int a = (inst >> 6) & 0xFF;
        int b = (inst >> 14) - 0x1ffff;
        writeint(a);
        writestr(" ");
        int gotopos = pc + b + 1;
        if (gotopos >= instcnt || gotopos < 0) {
            writeint(b);
        } else {
            writestr(":goto_");
            writeint(gotopos);
        }
        break;
    }
    case 33: {
        writestr("FORPREP v");
        int a = (inst >> 6) & 0xFF;
        int b = (inst >> 14) - 0x1ffff;
        writeint(a);
        writestr(" ");
        int gotopos = pc + b + 1;
        if (gotopos >= instcnt || gotopos < 0) {
            writeint(b);
        } else {     
            writestr(":goto_");
            writeint(gotopos);
        }
        break;
    }
    case 34: {
        writestr("TFORCALL v");
        int a = (inst >> 6) & 0xFF;
        int unused = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        writestr(" ");
        writeint(c);
        writestr(" ");
        writeint(unused);
        break;
    }
    case 35: {
        writestr("TFORLOOP v");
        int a = (inst >> 6) & 0xFF;
        int b = (inst >> 14) - 0x1ffff;
        writeint(a);
        writestr(" ");
        int gotopos = pc + b + 1;
        if (gotopos >= instcnt || gotopos < 0) {
            writeint(b);
        } else {
            writestr(":goto_");
            writeint(gotopos);
        }
        break;
    }
    case 36: {
        writestr("SETLIST v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b) {
            if (c) {
                //SETLIST A B C
                //for (i = 1; i<=B; i++) {
                //    vA[ (C-1)*50 + i ] = v(A + i);
                //}
                writestr(" ");
                writeint((c - 1) * 50 + 1);
                writestr(" v");
                writeint(a + b);
            } else {
                writestr(" GET_EXTRA");
                writestr(" v");
                writeint(a + b);
            }
        } else {
            if (c) {
                writestr(" ");
                writeint((c - 1) * 50 + 1);
                writestr(" GET_TOP");
            } else {
                writestr(" GET_EXTRA GET_TOP");
            }
        }
        break;
    }
    case 37: {
        writestr("CLOSURE v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 14;
        writeint(a);
        writestr(" F");
        writeint(b);
        break;
    }
    case 38: {
        writestr("VARARG");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        if (b) {
            writestr(" v");
            writeint(a);
            writestr("..v");
            writeint(a + b - 2);
        } else {
            writestr(" ");
            writeint(a);
            writestr(" SET_TOP");
        }
        if (unused) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 39: {
        writestr("EXTRAARG ");
        int a = inst >> 6;
        writeint(a);
        break;
    }
    case 40: {
        writestr("IDIV v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 41: {
        writestr("BNOT v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int unused = (inst >> 14) & 511;
        writeint(a);
        writestr(" v");
        writeint(b);
        if (unused != 0) {
            writestr(" ");
            writeint(unused);
        }
        break;
    }
    case 42: {
        writestr("BAND v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 43: {
        writestr("BOR v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 44: {
        writestr("BXOR v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 45: {
        writestr("SHL v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    case 46: {
        writestr("SHR v");
        int a = (inst >> 6) & 0xFF;
        int b = inst >> 23;
        int c = (inst >> 14) & 511;
        writeint(a);
        if (b > 255) {
            b = b & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(b);
        if (c > 255) {
            c = c & 0xff;
            writestr(" k");
        } else {
            writestr(" v");
        }
        writeint(c);
        break;
    }
    default: {
        writehexdword(inst);
        break;
    }
    }
}


