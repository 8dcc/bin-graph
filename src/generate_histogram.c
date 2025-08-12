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
#include <stdlib.h>

#include "include/generate.h"
#include "include/image.h"
#include "include/args.h"
#include "include/read_file.h"

Image* generate_histogram(const Args* args, ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;
    image_init(image, args, bytes->size);

    // FIXME
    assert(image->height == 256);

    uint8_t most_frequent = 0;
    uint32_t* occurrences = calloc(256, sizeof(uint32_t));
    if (occurrences == NULL)
        return NULL;

    /* Store the occurrences including the most frequent byte */
    for (size_t i = 0; i < bytes->size; i++) {
        const uint8_t byte = bytes->data[i];

        occurrences[byte]++;

        if (occurrences[byte] > occurrences[most_frequent])
            most_frequent = byte;
    }

    /*
     * Draw each horizontal line based on occurrences relative to the most
     * frequent byte.
     */
    for (size_t y = 0; y < image->height; y++) {
        const uint32_t line_width =
          occurrences[y] * image->width / occurrences[most_frequent];

        for (size_t x = 0; x < line_width; x++) {
            Color* color = &image->pixels[image->width * y + x];
            color->r = color->g = color->b = 0xFF;
        }
    }

    free(occurrences);
    return image;
}
