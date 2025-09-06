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
#include <stddef.h>
#include <stdlib.h>

#include "include/transform.h"
#include "include/args.h"
#include "include/image.h"
#include "include/util.h"

static bool validate_args(const Args* args) {
    switch (args->mode) {
        case ARGS_MODE_HISTOGRAM:
        case ARGS_MODE_ENTROPY_HISTOGRAM:
        case ARGS_MODE_BIGRAMS:
        case ARGS_MODE_DOTPLOT:
            WRN("The \"squares\" transformation is not recommended for the "
                "current mode (%s).",
                args_get_mode_name(args->mode));
            break;
        default:
            break;
    }
    return true;
}

void transform_squares(const Args* args, Image* image) {
    assert(args->transform_squares_side > 0);

    if (!validate_args(args))
        return;

    const int square_side     = args->transform_squares_side;
    const int square_size     = square_side * square_side;
    const size_t total_pixels = image->width * image->height;

    /*
     * Increase the width and height if they are not divisible by the square
     * side.
     */
    if (image->width % square_side != 0)
        image->width += square_side - image->width % square_side;
    if (image->height % square_side != 0)
        image->height += square_side - image->height % square_side;

    /* Number of squares in each row. Division should be exact now. */
    const size_t squares_per_row = image->width / square_side;

    /* Allocate the array with the new image dimensions */
    Color* new_pixels = calloc(image->height * image->width, sizeof(Color));
    if (new_pixels == NULL) {
        ERR("Failed to allocate new pixels array.");
        return;
    }

    /* Iterate the original pixels */
    for (size_t i = 0; i < total_pixels; i++) {
        /* Number and coordinates of the current square */
        const size_t square_num = i / square_size;
        const size_t square_y   = square_num / squares_per_row;
        const size_t square_x   = square_num % squares_per_row;

        /* Internal number and coordinates inside the current square */
        const size_t internal_num = i % square_size;
        const size_t internal_y   = internal_num / square_side;
        const size_t internal_x   = internal_num % square_side;

        /* Final coordinates in the image for the new point */
        const size_t final_y = square_side * square_y + internal_y;
        const size_t final_x = square_side * square_x + internal_x;

        /* Copy the color in the old position to the new one */
        new_pixels[image->width * final_y + final_x] = image->pixels[i];
    }

    /* Free the old pixel array and overwrite the pointer with the new one */
    free(image->pixels);
    image->pixels = new_pixels;
}
