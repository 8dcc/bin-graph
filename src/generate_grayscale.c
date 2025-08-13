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

#include <stdlib.h>

#include "include/generate.h"
#include "include/image.h"
#include "include/args.h"
#include "include/read_file.h"

Image* generate_grayscale(const Args* args, ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;
    image_init(image, args, bytes->size);

    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            /*
             * One-dimensional index for both the `bytes->data' and
             * `image->pixels' arrays.
             */
            const size_t raw_idx = image->width * y + x;

            /* Pointer to the current color in the Image */
            Color* color = &image->pixels[raw_idx];

            /*
             * If we are not in-bounds, we are filling the last row; use a
             * generic padding color.
             */
            if (raw_idx >= bytes->size) {
                color->r = color->g = color->b = 0x00;
                continue;
            }

            /* The color brightness is determined by the byte value */
            color->r = color->g = color->b = bytes->data[raw_idx];
        }
    }

    return image;
}
