#pragma once
#include <stdlib.h> 
#include <stdio.h>
#include <cstring>
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>


extern char* pos;
extern char* buf;
extern char* end;



inline void bufferinit() {
	buf = (char*)malloc(65536);
	if (buf == nullptr) {
		printf("Error: Failed to allocate memory");
		exit(1);
	}
	pos = buf;
	end = buf + 65536;
}


inline void writestr(const char *str, size_t sz) {
	memcpy(pos, str, sz);
	pos += sz;
}

template <size_t N>
inline void writestr(const char(&str)[N]) {
	memcpy(pos, str, N - 1);
	pos += N - 1;
}

inline void makeroom(size_t size) {
	if (pos + size > end) {
		size_t nsz = (end - buf) * 2;
		size_t usz = pos - buf;
		while (nsz <= size + usz) {
			nsz *= 2;
		}
		char* nbuf = (char*)realloc(buf, nsz);
		if (nbuf == nullptr) {
			printf("Error: Failed to reallocate buffer");
			free(buf);
			exit(1);
		}
		buf = nbuf;
		pos = nbuf + usz;
		end = nbuf + nsz;
	}
}

inline void writeindent(const char* indentpart, size_t size, size_t count) {
	pos[0] = '\n';
	pos++;
	for (size_t i = 0; i < count; i++) {
		for (size_t j = 0; j < size; j++) {
			pos[i + j] = indentpart[j];
		}
	}
	pos += size * count;
}

inline void writeindent(char indent, size_t count) {
	pos[0] = '\n';
	pos++;
	for (size_t i = 0; i < count; i++) {
		pos[i] = indent;
	}
	pos += count;
}

inline void writehexintadditional(int n) {
	char hex[] = "0123456789ABCDEF";
	pos[0] = '(';
	pos[1] = '0';
	pos[2] = 'x';
	pos[3] = hex[(n >> 28) & 0xf];
	pos[4] = hex[(n >> 24) & 0xf];
	pos[5] = hex[(n >> 20) & 0xf];
	pos[6] = hex[(n >> 16) & 0xf];
	pos[7] = hex[(n >> 12) & 0xf];
	pos[8] = hex[(n >> 8) & 0xf];
	pos[9] = hex[(n >> 4) & 0xf];
	pos[10] = hex[n & 0xf];
	pos[11] = ')';
	pos += 12;

}

template <typename T>
inline void writehexdword(T num) {
	char hex[] = "0123456789ABCDEF";
	pos[0] = '0';
	pos[1] = 'x';
	pos[2] = hex[(num >> 28) & 0xf];
	pos[3] = hex[(num >> 24) & 0xf];
	pos[4] = hex[(num >> 20) & 0xf];
	pos[5] = hex[(num >> 16) & 0xf];
	pos[6] = hex[(num >> 12) & 0xf];
	pos[7] = hex[(num >> 8) & 0xf];
	pos[8] = hex[(num >> 4) & 0xf];
	pos[9] = hex[num & 0xf];
	pos += 10;
}



inline void writeunused9(int num) {
	char hex[] = "0123456789ABCDEF";
	pos[0] = ' ';
	pos[1] = '(';
	pos[2] = '0';
	pos[3] = 'x';
	pos[4] = hex[(num >> 8) & 0xf];
	pos[5] = hex[(num >> 4) & 0xf];
	pos[6] = hex[num & 0xf];
	pos[7] = ')';
	pos += 8;
}



inline void writeunused18(int num) {
	char hex[] = "0123456789ABCDEF";
	pos[0] = ' ';
	pos[1] = '(';
	pos[2] = '0';
	pos[3] = 'x';
	pos[4] = hex[(num >> 16) & 0xf];
	pos[5] = hex[(num >> 12) & 0xf];
	pos[6] = hex[(num >> 8) & 0xf];
	pos[7] = hex[(num >> 4) & 0xf];
	pos[8] = hex[num & 0xf];
	pos[9] = ')';
	pos += 10;
}

inline void writehexbyte(unsigned char byte) {
	char hex[] = "0123456789ABCDEF";
	pos[0] = hex[(byte >> 4) & 0xF];
	pos[1] = hex[byte & 0xF];
	pos += 2;
}

inline void writehexbyteadditional(unsigned char byte) {
	char hex[] = "0123456789ABCDEF";
	pos[0] = '(';
	pos[1] = '0';
	pos[2] = 'x';
	pos[3] = hex[(byte >> 4) & 0xf];
	pos[4] = hex[byte & 0xf];
	pos[5] = ')';
	pos += 6;
}

inline void writeint(int n) {
	auto nstr = fmt::format_int(n);
	size_t size = nstr.size();
	memcpy(pos, nstr.data(), size);
	pos += size;
}

inline void writefloat(double f) {
	auto fstr = fmt::format("{}", f);
	size_t size = fstr.size();
	memcpy(pos, fstr.data(), size);
	pos += size;
}

inline char* getbuf() {
	return buf;
}

inline char* getpos() {
	return pos;
}

