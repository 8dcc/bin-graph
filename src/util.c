
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/util.h"

size_t get_file_size(FILE* fp) {
    fseek(fp, 0L, SEEK_END);
    const size_t file_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return file_sz;
}
