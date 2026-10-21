
#include "luac/file.hpp"
#include <cstdio>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: luacdis <file>");
        return 0;
    }
    Luac::File lf;
    lf.loadFromFile(argv[1]);
    char filename[1024];
    snprintf(filename, sizeof(filename), "%s.lasm", argv[1]);
    lf.printToFile(filename);
    return 0;
}
