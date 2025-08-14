#pragma once
#include "luaencode.h"
#include "iostream"
#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/format.h"
//#include "utils.hpp"
#include "writer.hpp"


unsigned char readbyte(unsigned char*& luacpos);

int readintlittle(unsigned char*& luacpos);
unsigned int readuintlittle(unsigned char*& luacpos);
int readintbig(unsigned char*& luacpos);
unsigned int readuintbig(unsigned char*& luacpos);
void readbytesluastring(unsigned char*& luacpos, int n);
int readsize4little(unsigned char*& luacpos);
int readsize4big(unsigned char*& luacpos);
int readsize8little(unsigned char*& luacpos);
int readsize8big(unsigned char*& luacpos);
void readluanumibig(unsigned char*& luacpos);
void readluanumilittle(unsigned char*& luacpos);
void readluaintbig(unsigned char*& luacpos);
void readluaintlittle(unsigned char*& luacpos);
void readluanumdlittle(unsigned char*& luacpos);
void readluanumdbig(unsigned char*& luacpos);
void disasm(unsigned int inst, int instcnt, int pc);

//void disasminstruction(unsigned char* luacpos, int inst);