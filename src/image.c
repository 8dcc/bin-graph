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

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/image.h"
#include "include/args.h"
#include "include/byte_array.h"
#include "include/util.h"

bool image_init(Image* image, size_t width, size_t height) {
    assert(image != NULL);

    image->width  = width;
    image->height = height;

    image->pixels = calloc(image->height * image->width, sizeof(Color));
    if (image->pixels == NULL)
        return false;

    return true;
}

void image_deinit(Image* image) {
    free(image->pixels);
    image->pixels = NULL;
}
