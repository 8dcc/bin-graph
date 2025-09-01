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
#include "include/byte_array.h"
#include "include/util.h"

static bool validate_args(const Args* args) {
    if (args->block_size != ARGS_DEFAULT_BLOCK_SIZE)
        WRN("The current mode (%s) is not affected by the user-specified block "
            "size (%zu).",
            args_get_mode_name(args->mode),
            args->block_size);
    return true;
}

static inline Image* alloc_and_init_image(const Args* args) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;

    const size_t width  = args->output_width;
    const size_t height = 256;
    if (!image_init(image, width, height))
        return NULL;

    return image;
}

/*----------------------------------------------------------------------------*/

Image* generate_histogram(const Args* args, ByteArray* bytes) {
    if (!validate_args(args))
        return NULL;

    Image* image = alloc_and_init_image(args);
    if (image == NULL)
        return NULL;
    assert(image->height == 256);

    size_t* occurrences = calloc(256, sizeof(size_t));
    if (occurrences == NULL)
        return NULL;

    /*
     * Count the number of occurrences of each byte, also storing which is the
     * most frequent byte.
     */
    uint8_t most_frequent = 0;
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
        const size_t line_width =
          occurrences[y] * image->width / occurrences[most_frequent];

        for (size_t x = 0; x < line_width; x++) {
            Color* color = &image->pixels[image->width * y + x];
            color->r = color->g = color->b = 0xFF;
        }
    }

    free(occurrences);
    return image;
}
