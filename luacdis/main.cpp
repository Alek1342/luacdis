#include "main.hpp"





int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage : %s <file>", argv[0]);
        return 0;
    }

    FILE* tmpfile;
    char* luacfilename = argv[1];

    if (fopen_s(&tmpfile, luacfilename, "rb") != 0) {
        printf("Error: Failed to open %s for reading bytes", argv[1]);
        return 1;
    }
    fseek(tmpfile, 0, SEEK_END);
    size_t luacsz = ftell(tmpfile);
    if (luacsz < 18) {
        printf("Error: File too small");
        fclose(tmpfile);
        return 1;
    }
    fseek(tmpfile, 0, SEEK_SET);
    unsigned char* luac = (unsigned char*)malloc(luacsz);
    if (luac == nullptr) {
        printf("Error: Failed to allocate memory");
        fclose(tmpfile);
        return 1;
    }
    fread(luac, luacsz, 1, tmpfile);
    fclose(tmpfile);
    unsigned char* luacpos = luac;
    unsigned char* luacend = luac + luacsz;
    bufferinit();


    unsigned char sig[] = { '\033', 'L', 'u', 'a' };
    for (size_t i = 0; i < 4; ++i) {
        if (luacpos[i] != sig[i]) {
            printf("Error: No lua signature");
            return 1;
        }
    }
    luacpos += 4;
    writestr("LUAC_VERSION = 0x");
    writehexbyte(readbyte(luacpos));
    writestr("\n");
    writestr("LUAC_FORMAT = 0x");
    writehexbyte(readbyte(luacpos));
    writestr("\n");
    writestr("IS_LITTLE_ENDIAN = 0x");
    unsigned char islittleendianbyte = readbyte(luacpos);
    writehexbyte(islittleendianbyte);
    writestr("\n");
    writestr("SIZEOF_INT = 0x");
    writehexbyte(readbyte(luacpos));
    writestr("\n");
    writestr("SIZEOF_SIZET = 0x");
    unsigned char sizetsizebyte = readbyte(luacpos);
    writehexbyte(sizetsizebyte);
    writestr("\n");
    writestr("SIZEOF_INSTRUCTION = 0x");
    writehexbyte(readbyte(luacpos));
    writestr("\n");
    writestr("SIZEOF_LUA_NUMBER = 0x");
    writehexbyte(readbyte(luacpos));
    writestr("\n");
    writestr("NUMBER_FORMAT = 0x");
    unsigned char numberformatbyte = readbyte(luacpos);
    writehexbyte(numberformatbyte);
    writestr("\n");
    unsigned char headertail[] = { '\x19', '\x93', '\x0d', '\x0a',  '\x1a', '\x0a' };
    for (size_t i = 0; i < 6; ++i) {
        if (luacpos[i] != headertail[i]) {
            printf("Error: No luac header tail");
            return 1;
        }
    }
    luacpos += 6;
    int sizetsize;

    int (*readint)(unsigned char*&);
    unsigned int (*readuint)(unsigned char*&);
    int (*readsize)(unsigned char*&);
    void (*readluanum)(unsigned char*&);
    void (*readluaint)(unsigned char*&);

    if (!(numberformatbyte == 0 || numberformatbyte == 1 || numberformatbyte == 4)) {
        printf("Error: Wrong number format");
        return 1;
    }
    if (islittleendianbyte) {
        readint = readintlittle;
        readuint = readuintlittle;
        readluaint = readluaintlittle;
        if (sizetsizebyte != 8) {
            readsize = readsize4little;
            sizetsize = 4;
        } else {
            readsize = readsize8little;
            sizetsize = 8;
        }
        if (numberformatbyte == 1) {
            readluanum = readluanumilittle;
        } else {
            readluanum = readluanumdlittle;
        }
    } else {
        readint = readintbig;
        readuint = readuintbig;
        readluaint = readluaintbig;
        if (sizetsizebyte != 8) {
            readsize = readsize4big;
            sizetsize = 4;
        } else {
            readsize = readsize8big;
            sizetsize = 8;
        }
        if (numberformatbyte == 1) {
            readluanum = readluanumibig;
        } else {
            readluanum = readluanumdbig;
        }
    }
    int* stack = (int*)malloc(256);
    size_t stacksize = 256;
    size_t stacktop = 0;
    int* stack2 = (int*)malloc(256);
    size_t stacksize2 = 256;
    size_t stacktop2 = 0;
    bool readcode = true;
    size_t sizetmp;
    int inttmp;
    int inttmp2;
    int funccnt = 1;
    int funcn = 0;
    size_t gotossize;
    size_t gotoscount;
    int jmp;
  
    int* gotospos = nullptr;
 
    unsigned int uinttmp;

    unsigned char uchartmp;
    const char* indentpart = "\t";
    size_t indentpsize = 1;
    size_t i1sz;
    size_t i2sz;
    size_t i1cn;
    while (true) {
        i1cn = stacktop + 1;
        i1sz = i1cn * indentpsize + 1;
        i2sz = stacktop * indentpsize + 1;
        if (readcode) {
            if (luacpos + 15 >= luacend) {
                printf("Error: Unexpected end of luac at function header");
                return 1;
            }
            makeroom(i2sz + i1sz * 5 + 115);

            writeindent(indentpart, indentpsize, stacktop);
            writestr(".func F");
            writeint(funcn);

            writeindent(indentpart, indentpsize, i1cn);
            writestr(".linedefined ");
            writeint(readint(luacpos));
         

            writeindent(indentpart, indentpsize, i1cn);
            writestr(".lastlinedefined ");
            writeint(readint(luacpos));

            writeindent(indentpart, indentpsize, i1cn);
            writestr(".numparams ");
            writeint(readbyte(luacpos));

            writeindent(indentpart, indentpsize, i1cn);
            writestr(".is_vararg ");
            writeint(readbyte(luacpos));

            writeindent(indentpart, indentpsize, i1cn);
            writestr(".maxstacksize ");
            writeint(readbyte(luacpos));
            
            inttmp = readint(luacpos);
            if (inttmp < 0) {
                printf("Error: Negative instruction count");
                return 1;
            }
            if (luacpos + (size_t)inttmp * 4 >= luacend) {
                printf("Error: Unexpected end of luac at function instrunctions");
                return 1;
            }
            int* gotos = (int*)malloc(65536);
            gotoscount = 0;
            gotossize = 65536;
            unsigned int* code = (unsigned int*)malloc(((size_t)inttmp)*4);
            for (int i = 0; i < inttmp; i++) {
                code[i] = readuint(luacpos);
            }
            for (int i = 0; i < inttmp; i++) {
                uinttmp = code[i];
                switch (code[i] & 0x3F) {
                case 23:
                case 32: 
                case 33: 
                case 35: 
                    inttmp2 = i + (int)(uinttmp >> 14) - 131070;
                    if (inttmp2 >= inttmp || inttmp2 < 0) {
                        break;
                    }
                    stackpush(gotos, gotoscount, gotossize, inttmp2);
                    break;
                default: 
                    break;

                }
            }
            if (gotoscount) {
                std::sort(gotos, gotos + gotoscount);
                gotoscount = removeDuplicates(gotos, gotoscount);
                gotospos = gotos;
                jmp = gotospos[0];
                gotospos += 1;
            }
            makeroom((size_t)inttmp * 2 * (i1sz + 40));
            for (int i = 0; i < inttmp; ++i) {
                if (gotoscount && jmp == i) {
                    writeindent(indentpart, indentpsize, i1cn);
                    writestr(":goto_");
                    writeint(jmp);
                    gotoscount--;
                    if (gotoscount) {
                        jmp = gotospos[0];
                        gotospos++;
                    }
                }
                writeindent(indentpart, indentpsize, i1cn);
                disasm(code[i], inttmp, i);
                
            }
            free(code);
            free(gotos);
            if (luacpos + 4 >= luacend) {
                printf("Error: Unexpected end of luac at constant count");
                return 1;
            }
            inttmp = readint(luacpos);
            if (inttmp < 0) {
                makeroom(i1sz + 20);
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".constcnt ");
                writehexdword(inttmp);
                inttmp = 0;
            }
            makeroom((size_t)inttmp* (i1sz+20));
            for (int i = 0; i < inttmp; i++) {
                writeindent(indentpart, indentpsize, i1cn);
                if (luacpos + 9 >= luacend) {
                    printf("Error: Unexpected end of luac at type of const");
                    return 1;
                }
                uchartmp = readbyte(luacpos);
                switch (uchartmp) {
                case 0: {
                    writestr("nil");
                    break;
                }
                case 1: {
                    uchartmp = readbyte(luacpos);
                    if (uchartmp) {
                        if (uchartmp != 1) {
                            writehexbyteadditional(uchartmp);
                        }
                        writestr("true");
                    } else {
                        writestr("false");
                    }
                    break;

                }
                case 3: {
                    readluanum(luacpos);
                    break;
                }
                case 4: {
                    inttmp2 = readsize(luacpos);
                    if (inttmp2 < 0) {
                        printf("Error: Negative string size");
                        return 1;
                    }
                    if (luacpos + inttmp2 >= luacend) {
                        printf("Error: Unexpected end of luac at string const");
                        return 1;
                    }
                    sizetmp = 8 + 4 * (size_t)inttmp2;
                    makeroom(sizetmp);
                    if (inttmp2 == 0) {
                        writestr("null");
                    } else {
                        writestr("\"");
                        readbytesluastring(luacpos, inttmp2 - 1);
                        writestr("\"");
                        uchartmp = readbyte(luacpos);
                        if (uchartmp != 0) {
                            writehexbyteadditional(uchartmp);
                        }
                    }
                    break;
                }
                case 254: {
                    readluaint(luacpos);
                    break;
                }
                default: {
                    printf("Error: Wrong constant type");
                    return 1;
                }
                }
            }


            if (luacpos + 4 >= luacend) {
                printf("Error: Unexpected end of luac at func count");
                return 1;
            }
            inttmp = readint(luacpos);
            if (inttmp > 0) {
                stackpush(stack, stacktop, stacksize, funccnt);
                stackpush(stack2, stacktop2, stacksize2, funcn);
                funcn = 0;
                funccnt = inttmp;
            } else {
                if (inttmp < 0) {
                    makeroom(i1sz + 19);
                    writeindent(indentpart, indentpsize, i1cn);
                    writestr(".funccnt ");
                    writehexdword(inttmp);
                }
                readcode = false;
            }








        } else {




            if (luacpos + 4 >= luacend) {
                printf("Error: Unexpected end of luac at count of upvals");
                return 1;
            }
            inttmp = readint(luacpos);
            if (inttmp < 0) {
                makeroom(i1sz + 20);
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".upvalcnt ");
                writehexdword(inttmp);
                inttmp = 0;
            }
            if (luacpos + 2 * inttmp >= luacend) {
                printf("Error: Unexpected end of luac at upvals");
                return 1;
            }
            makeroom((size_t)inttmp * (i1sz + 17));
            for (int i = 0; i < inttmp; i++) {
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".upval ");
                uchartmp = readbyte(luacpos);
                if (uchartmp) {
                    if (uchartmp != 1) {
                        writehexbyteadditional(uchartmp);
                    }
                    writestr("v");
                } else {
                    writestr("u");
                }
                writeint(readbyte(luacpos));
            }


            if (luacpos + sizetsize >= luacend) {
                printf("Error: Unexpected end of luac at size of source str");
                return 1;
            }
            makeroom(i1sz + 20);
            if (sizetsize == 4) {
                inttmp = readsize(luacpos);
                if (inttmp < 0) {
                    printf("Error: Negative string size");
                    return 1;
                }
                if (luacpos + inttmp >= luacend) {
                    printf("Error: Unexpected end of luac at string");
                    return 1;
                }
                if (inttmp != 0) {
                    writeindent(indentpart, indentpsize, i1cn);
                    writestr(".source ");
                    sizetmp = 8 + 4 * (size_t)inttmp;
                    makeroom(sizetmp);
                    writestr("\"");
                    readbytesluastring(luacpos, inttmp - 1);
                    writestr("\"");
                    uchartmp = readbyte(luacpos);
                    if (uchartmp != 0) {
                        writehexbyteadditional(uchartmp);
                    }
                }
            } else {
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".source ");
                inttmp = readsize(luacpos);
                if (inttmp < 0) {
                    printf("Error: Negative string size");
                    return 1;
                }
                if (luacpos + inttmp >= luacend) {
                    printf("Error: Unexpected end of luac at string");
                    return 1;
                }
                sizetmp = 8 + 4 * (size_t)inttmp;
                makeroom(sizetmp);
                if (inttmp == 0) {
                    writestr("null");
                } else {
                    writestr("\"");
                    readbytesluastring(luacpos, inttmp - 1);
                    writestr("\"");
                    uchartmp = readbyte(luacpos);
                    if (uchartmp != 0) {
                        writehexbyteadditional(uchartmp);
                    }
                }
            }
            
            if (luacpos + 4 >= luacend) {
                printf("Error: Unexpected end of luac at count of lineinfo");
                return 1;
            }
            inttmp = readint(luacpos);
            if (inttmp < 0) {
                printf("Error: Negative lineinfo count");
                return 1;
            }
            if (luacpos + (size_t)inttmp * 4 >= luacend) {
                printf("Error: Unexpected end of luac at lineinfos");
                return 1;
            }
            makeroom((size_t)inttmp * (i1sz + 21));
            for (int i = 0; i < inttmp; i++) {
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".lineinfo ");
                writeint(readint(luacpos));
            }
            
            

            if (luacpos + 4 >= luacend) {
                printf("Error: Unexpected end of luac at count of locvars");
                return 1;
            }
            inttmp = readint(luacpos);
            if (inttmp < 0) {
                makeroom(i1sz + 21);
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".locvarcnt ");
                writehexdword(inttmp);
                inttmp = 0;
            }
            makeroom((size_t)inttmp * (i1sz + 31));
            for (int i = 0; i < inttmp; i++) {
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".locvar ");
                if (luacpos + sizetsize >= luacend) {
                    printf("Error: Unexpected end of luac at size of locvar name");
                    return 1;
                }
                inttmp2 = readsize(luacpos);
                if (inttmp2 < 0) {
                    printf("Error: Negative string size");
                    return 1;
                }
                if (luacpos + inttmp2 >= luacend) {
                    printf("Error: Unexpected end of luac at string");
                    return 1;
                }
                sizetmp = 8 + 4 * (size_t)inttmp2;
                makeroom(sizetmp);
                if (inttmp2 == 0) {
                    writestr("null");
                } else {
                    writestr("\"");
                    readbytesluastring(luacpos, inttmp2 - 1);
                    writestr("\"");
                    uchartmp = readbyte(luacpos);
                    if (uchartmp != 0) {
                        writehexbyteadditional(uchartmp);
                    }
                }
                writestr(" ");
                writeint(readint(luacpos));
                writestr(" ");
                writeint(readint(luacpos));
            }


            if (luacpos + 4 > luacend) {
                printf("Error: Unexpected end of luac at count of upnames");
                return 1;
            }
            inttmp = readint(luacpos);
            if (inttmp < 0) {
                makeroom(i1sz + 21);
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".upnamecnt ");
                writehexdword(inttmp);
                inttmp = 0;
            }
            makeroom((size_t)inttmp* (i1sz+20));
            for (int i = 0; i < inttmp; i++) {
                if (luacpos + sizetsize >= luacend) {
                    printf("Error: Unexpected end of luac at size of upname");
                    return 1;
                }
                writeindent(indentpart, indentpsize, i1cn);
                writestr(".upname ");
                inttmp2 = readsize(luacpos);
                if (inttmp2 < 0) {
                    printf("Error: Negative string size");
                    return 1;
                }
                if (luacpos + inttmp2 > luacend) {
                    printf("Error: Unexpected end of luac at string const");
                    return 1;
                }
                sizetmp = 8 + 4 * (size_t)inttmp2;
                makeroom(sizetmp);
                if (inttmp2 == 0) {
                    writestr("null");
                } else {
                    writestr("\"");
                    readbytesluastring(luacpos, inttmp2 - 1);
                    writestr("\"");
                    uchartmp = readbyte(luacpos);
                    if (uchartmp != 0) {
                        writehexbyteadditional(uchartmp);
                    }
                }
            }
            makeroom(i2sz + 4);
            writeindent(indentpart, indentpsize, stacktop);
            writestr(".end");
            funccnt--;
            funcn++;
            if (funccnt == 0) {
                if (stacktop == 0) {
                    break;
                }
                funccnt = stackpop(stack, stacktop);
                funcn = stackpop(stack2, stacktop2);

            } else {
                readcode = true;
            }
        }
    }
    free(stack);
    size_t luacfilenamelen = strlen(luacfilename);
    size_t tailsize = luacend - luacpos;
    if (tailsize > 0) {
        if (tailsize > 8000000) {
            char* tailfilename = (char*)malloc(luacfilenamelen + 6);
            memcpy(tailfilename, luacfilename, luacfilenamelen);
            memcpy(tailfilename + luacfilenamelen, ".tail", 6);
            if (fopen_s(&tmpfile, tailfilename, "wb") != 0) {
                printf("Error: Failed to open %s for writing bytes", argv[1]);
                return 1;
            }
            fwrite(luacpos, tailsize, 1, tmpfile);
            fclose(tmpfile);
        } else {
            makeroom(tailsize * 4 + 9);
            writestr(".tail \"");
            readbytesluastring(luacpos, tailsize);
            writestr("\"\n");
        }
    }
    free(luac);
    char* llasmfilename = (char*)malloc(luacfilenamelen + 6);
    memcpy(llasmfilename, luacfilename, luacfilenamelen);
    memcpy(llasmfilename + luacfilenamelen, ".lasm", 6);
    if (fopen_s(&tmpfile, llasmfilename, "wb") != 0) {
        printf("Error: Failed to open %s for writing bytes", argv[1]);
        return 1;
    }
    char* buf = getbuf();
    char* pos = getpos();
    fwrite(buf, pos - buf, 1, tmpfile);
    fclose(tmpfile);
    free(buf);
    return 0;
}