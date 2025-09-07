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

#include "include/transform.h"
#include "include/args.h"
#include "include/image.h"
#include "include/util.h"

bool transform_zigzag(const Args* args, Image* image) {
    UNUSED(args);

    for (size_t y = 0; y < image->height; y++) {
        /* Don't reverse even rows */
        if (y % 2 == 0)
            continue;

        /* Reverse odd rows, swapping bytes from the start and the end */
        if (reverse_buffer(&image->pixels[image->width * y],
                           image->width,
                           sizeof(Color)) == NULL)
            break;
    }

    return true;
}
