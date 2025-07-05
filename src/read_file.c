/*
 * Copyright 2025 8dcc
 *
 * This file is part of bin-graph.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

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

void get_real_offsets(FILE* fp, long* offset_start, long* offset_end) {
    const long file_sz = get_file_size(fp);

    /* Make sure the offsets are not out of bounds */
    if (*offset_start > file_sz || *offset_end > file_sz)
        DIE("An offset (%zX, %zX) was bigger than the file size (%zX)",
            *offset_start,
            *offset_end,
            file_sz);

    /* If the global offset is zero, it means end of the file. Overwrite it */
    if (*offset_end == 0)
        *offset_end = file_sz;

    if (*offset_end <= *offset_start)
        DIE("End offset (%zX) was smaller or equal than the start offset (%zX)",
            *offset_end,
            *offset_start);
}

void read_file(ByteArray* dst, FILE* fp, long offset_start, long offset_end) {
    /* Ensure these are real and valid offsets */
    get_real_offsets(fp, &offset_start, &offset_end);

    /* Move to the starting offset */
    if (fseek(fp, offset_start, SEEK_SET) != 0)
        DIE("fseek() failed with offset 0x%zX. Errno: %s (%d)",
            offset_start,
            strerror(errno),
            errno);

    /* Read the bytes from the file */
    for (size_t i = 0; i < dst->size; i++)
        dst->data[i] = fgetc(fp);
}

/*----------------------------------------------------------------------------*/

void byte_array_init(ByteArray* bytes,
                     FILE* fp,
                     long offset_start,
                     long offset_end) {
    /* Ensure these are real and valid offsets */
    get_real_offsets(fp, &offset_start, &offset_end);

    bytes->size = offset_end - offset_start;
    bytes->data = calloc(bytes->size, sizeof(uint8_t));
    if (bytes->data == NULL)
        DIE("Failed to allocate the byte array (%zu bytes)", bytes->size);
}

void byte_array_free(ByteArray* bytes) {
    free(bytes->data);
    bytes->data = NULL;
}
