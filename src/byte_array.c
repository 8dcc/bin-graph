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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/byte_array.h"

bool byte_array_init(ByteArray* array, size_t size) {
    array->size = size;
    array->data = calloc(array->size, sizeof(uint8_t));
    if (array->data == NULL)
        return false;

    return true;
}

bool byte_array_resize(ByteArray* array, size_t new_size) {
    void* new_ptr = realloc(array->data, new_size);
    if (new_ptr == NULL)
        return false;

    array->data = new_ptr;
    array->size = new_size;
    return true;
}

void byte_array_destroy(ByteArray* array) {
    if (array->data != NULL) {
        free(array->data);
        array->data = NULL;
    }
}
