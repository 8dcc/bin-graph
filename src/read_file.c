
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/util.h"

long get_file_size(FILE* fp) {
    fseek(fp, 0L, SEEK_END);
    const long file_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return file_sz;
}

ByteArray read_file(FILE* fp, long offset_start, long offset_end) {
    const long file_sz = get_file_size(fp);

    /* Make sure the offsets are not out of bounds */
    if (offset_start > file_sz || offset_end > file_sz)
        die("An offset (%zX, %zX) was bigger than the file size (%zX).",
            offset_start, offset_end, file_sz);

    /* If the global offset is zero, read until the end of the file */
    if (offset_end == 0)
        offset_end = file_sz;

    if (offset_end <= offset_start)
        die("End offset (%zX) was smaller or equal than the start offset "
            "(%zX).",
            offset_end, offset_start);

    /* Move to the starting offset */
    if (fseek(fp, offset_start, SEEK_SET) != 0)
        die("fseek() failed with offset 0x%zX. Errno: %s (%d).", offset_start,
            strerror(errno), errno);

    /* The size of the section that we are trying to read */
    const long input_sz = offset_end - offset_start;

    /* Calculate the number of samples we will use for generating the image */
    /* TODO: Remove g_sample_step entirely */
    ByteArray result;
    result.size = input_sz / g_sample_step;
    if (input_sz % g_sample_step != 0)
        result.size++;

    /* Allocate the array for the samples */
    result.data = malloc(result.size);
    if (result.data == NULL)
        die("Failed to allocate the samples array (%zu bytes).", result.size);

    for (size_t i = 0; i < result.size; i++) {
        /* Store the first byte of the chunk */
        const int byte = fgetc(fp);

        /* Consume the rest of the chunk */
        for (size_t j = 1;
             j < g_sample_step && (i * g_sample_step) + j < input_sz; j++)
            fgetc(fp);

        assert(byte <= 0xFF);
        result.data[i] = byte;
    }

    return result;
}
