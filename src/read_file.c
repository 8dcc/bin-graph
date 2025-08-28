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
#include <stdio.h>
#include <assert.h>

#include "include/read_file.h"
#include "include/args.h"
#include "include/byte_array.h"
#include "include/util.h"

bool read_file(ByteArray* dst,
               FILE* fp,
               size_t offset_start,
               size_t offset_end) {
    assert(offset_end >= offset_start);

    /* Allocate and initialize the 'ByteArray' structure */
    const size_t initial_size =
      (offset_end == 0) ? 255 : offset_end - offset_start;
    byte_array_init(dst, initial_size);

    /* Skip initial bytes */
    int last_char = 0;
    for (size_t i = 0; i < offset_start && last_char != EOF; i++)
        last_char = fgetc(fp);

    /* Read the target bytes from the file, resizing it dynamically */
    size_t i;
    for (i = 0; last_char != EOF && (offset_end == 0 || i < offset_end); i++) {
        if (i >= dst->size && !byte_array_resize(dst, dst->size * 2))
            return false;

        dst->data[i] = last_char = fgetc(fp);
    }

    /* Overwrite with the actual length */
    assert(i <= dst->size);
    dst->size = i;

    return true;
}
