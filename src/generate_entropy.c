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
#include <limits.h>
#include <math.h> /* pow() */

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

    size_t width  = args->output_width;
    size_t height = bytes->size / width;
    if (bytes->size % width != 0)
        height++;

    if (!image_init(image, width, height))
        return NULL;

    return image;
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
        const uint8_t color_intensity = block_entropy * 255 / 8;

        /* Render this block with the same color */
        for (size_t j = 0; j < real_block_size; j++) {
            Color* color = &image->pixels[i + j];

#ifdef BIN_GRAPH_HEATMAP
            /*
             * The heatmap visualization uses a linear scale for blue values,
             * but an exponential scale for red values. This representation is
             * more closely related to how entropy is calculated (using a base 2
             * logarithm); in other words, brighter values are exponentially
             * more significant/informative than darker values.
             */
            color->r =
              (uint8_t)(pow((double)color_intensity / UCHAR_MAX, 3) * 255.0);
            color->g = 0;
            color->b = color_intensity;
#else  /* not BIN_GRAPH_HEATMAP */
            color->r = color->g = color->b = color_intensity;
#endif /* not BIN_GRAPH_HEATMAP */
        }
    }

    return image;
}
