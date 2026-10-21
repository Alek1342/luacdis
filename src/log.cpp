#include "log.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

void log(int level, const char* format, ...) {
    va_list args;
    va_start(args, format);

    switch (level) {
        case FATAL:
            fprintf(stdout, "[F] ");
            break;
        case ERROR:
            fprintf(stdout, "[E] ");
            break;
        case WARN:
            fprintf(stdout, "[W] ");
            break;
        case INFO:
            fprintf(stdout, "[I] ");
            break;
        case DEBUG:
            fprintf(stdout, "[D] ");
            break;
        case TRACE:
            fprintf(stdout, "[T] ");
            break;
    }

    vfprintf(stdout, format, args);

    va_end(args);

    if (level == FATAL) {
        exit(EXIT_FAILURE);
    }
}