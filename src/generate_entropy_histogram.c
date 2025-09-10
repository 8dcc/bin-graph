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

#include "include/generate.h"
#include "include/image.h"
#include "include/args.h"
#include "include/byte_array.h"
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

    /* Each row in the Y axis corresponds to an entropy block */
    size_t width  = args->output_width;
    size_t height = bytes->size / args->block_size;
    if (bytes->size % args->block_size != 0)
        height++;

    if (!image_init(image, width, height))
        return NULL;

    return image;
}

/*----------------------------------------------------------------------------*/

Image* generate_entropy_histogram(const Args* args, ByteArray* bytes) {
    if (!validate_args(args))
        return NULL;

    Image* image = alloc_and_init_image(args, bytes);
    if (image == NULL)
        return NULL;

    for (size_t y = 0; y < image->height; y++) {
        /* Get the raw data index of the current block */
        const size_t block_start = y * args->block_size;

        /* Make sure we are not reading past the end of 'bytes->size' */
        const size_t real_block_size =
          (block_start + args->block_size < bytes->size)
            ? args->block_size
            : bytes->size - block_start;

        /* Calculate the Shannon entropy for this block */
        const double block_entropy =
          entropy(&bytes->data[block_start], real_block_size);

        /*
         * Convert the entropy to a percentage, dividing it by its maximum
         * value. Then, similarly to the "histogram" mode, calculate the row
         * width based on the percentage.
         */
        const double entropy_percent = block_entropy / MAX_ENTROPY;
        const size_t line_width      = entropy_percent * image->width;

#ifdef BIN_GRAPH_ENTROPY_HISTOGRAM_DOTS
        Color* color = &image->pixels[image->width * y + line_width];
        color->r = color->g = color->b = 0xFF;
#else  /* not BIN_GRAPH_ENTROPY_HISTOGRAM_DOTS */
        for (size_t x = 0; x < line_width; x++) {
            Color* color = &image->pixels[image->width * y + x];
            color->r = color->g = color->b = 0xFF;
        }
#endif /* not BIN_GRAPH_ENTROPY_HISTOGRAM_DOTS */
    }

    return image;
}
