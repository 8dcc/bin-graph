
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/util.h"

void die(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);

    va_end(va);
    exit(1);
}

size_t get_file_size(FILE* fp) {
    fseek(fp, 0L, SEEK_END);
    const size_t file_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return file_sz;
}
