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
#include <stdlib.h>
#include <math.h> /* log2() */

#include "include/generate.h"
#include "include/image.h"
#include "include/args.h"
#include "include/read_file.h"
#include "include/util.h"

static bool validate_args(const Args* args) {
    if (args->block_size <= 1) {
        ERR("The current block size (%zu) is too small for the current mode "
            "(%s).",
            args->block_size,
            args_get_mode_name(args->mode));
        return false;
    }
    return true;
}

static inline Image* alloc_and_init_image(const Args* args, ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;

    size_t width  = args->output_width;
    size_t height = bytes->size / width;
    if (bytes->size % width != 0)
        height++;

    if (!image_init(image, width, height))
        return NULL;

    return image;
}

/*
 * Calculate the Shannon entropy of the specified bytes. Since log2() is used,
 * the return value is in the [0..8] range.
 *
 * For more information, see my article about entropy:
 * https://8dcc.github.io/programming/understanding-entropy.html
 */
static double entropy(void* data, size_t data_sz) {
    size_t* occurrences = calloc(256, sizeof(size_t));

    /* Count the occurrences of each byte in the input */
    for (size_t i = 0; i < data_sz; i++) {
        const uint8_t byte = ((uint8_t*)data)[i];
        occurrences[byte]++;
    }

    double result = 0.0;
    for (int byte = 0; byte < 256; byte++) {
        if (occurrences[byte] == 0)
            continue;

        /* Probablity of encountering this byte on the input */
        const double probability = (double)occurrences[byte] / data_sz;

        /* Accumulate entropy of each possible value (00..FF) */
        result -= probability * log2(probability);
    }

    free(occurrences);
    return result;
}

/*----------------------------------------------------------------------------*/

Image* generate_entropy(const Args* args, ByteArray* bytes) {
    if (!validate_args(args))
        return NULL;

    Image* image = alloc_and_init_image(args, bytes);
    if (image == NULL)
        return NULL;

    /* Iterate blocks of the input, each will share the same entropy color */
    for (size_t i = 0; i < bytes->size; i += args->block_size) {
        /* Make sure we are not reading past the end of 'bytes->size' */
        const size_t real_block_size = (i + args->block_size < bytes->size)
                                         ? args->block_size
                                         : bytes->size - i;

        /* Calculate the Shannon entropy for this block */
        const double block_entropy = entropy(&bytes->data[i], real_block_size);

        /*
         * Calculate the [00..FF] color for this block based on the [0..8]
         * entropy.
         */
        uint8_t color_intensity = block_entropy * 255 / 8;

        /* Render this block with the same color */
        for (size_t j = 0; j < real_block_size; j++) {
            Color* color = &image->pixels[i + j];
            color->r = color->g = color->b = color_intensity;
        }
    }

    return image;
}
