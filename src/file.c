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

#include "include/file.h"
#include "include/args.h"
#include "include/byte_array.h"
#include "include/util.h"

#define STDIN_FILENAME  "-"
#define STDOUT_FILENAME "-"

FILE* file_open(const char* path, enum EFileOpenMode mode) {
    switch (mode) {
        case FILE_MODE_READ:
            if (strcmp(path, STDIN_FILENAME) == 0)
                return stdin;
            else
                return fopen(path, "rb");

        case FILE_MODE_WRITE:
            if (strcmp(path, STDOUT_FILENAME) == 0)
                return stdout;
            else
                return fopen(path, "wb");

        default:
            DIE("Invalid mode enumerator. Aborting.");
    }
}

bool file_read(ByteArray* dst,
               FILE* fp,
               size_t offset_start,
               size_t offset_end) {
    const bool has_offset_end = (offset_end != 0);
    assert(!has_offset_end || offset_end >= offset_start);

    /* Allocate and initialize the 'ByteArray' structure */
    const size_t initial_size =
      (offset_end == 0) ? 255 : offset_end - offset_start;
    byte_array_init(dst, initial_size);

    /* Skip initial bytes */
    size_t file_pos;
    int last_char = 0;
    for (file_pos = 0; file_pos < offset_start && last_char != EOF; file_pos++)
        last_char = fgetc(fp);

    /* Read the target bytes from the file, resizing it dynamically */
    size_t dst_pos;
    for (dst_pos = 0; (!has_offset_end || file_pos < offset_end);
         dst_pos++, file_pos++) {
        if (dst_pos >= dst->size && !byte_array_resize(dst, dst->size * 2))
            return false;

        last_char = fgetc(fp);
        if (last_char == EOF)
            break;

        dst->data[dst_pos] = last_char;
    }

    /* Overwrite with the actual length */
    assert(dst_pos <= dst->size);
    dst->size = dst_pos;

    return true;
}
