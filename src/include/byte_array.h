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

#ifndef BYTE_ARRAY_H_
#define BYTE_ARRAY_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct ByteArray {
    uint8_t* data;
    size_t size;
} ByteArray;

/*
 * Initialize a 'ByteArray' structure with the specified size. The caller is
 * responsible for deinitializing the structure with 'byte_array_destroy'.
 */
bool byte_array_init(ByteArray* array, size_t size);

/*
 * Destroy a 'ByteArray' structure, freeing its members. Doesn't free the
 * 'ByteArray' structure itself.
 */
void byte_array_destroy(ByteArray* array);


#endif /* BYTE_ARRAY_H_ */
