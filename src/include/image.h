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

#ifndef IMAGE_H_
#define IMAGE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "args.h" /* Args */

typedef struct Color {
    uint8_t r, g, b;
} Color;

typedef struct Image {
    Color* pixels;        /* RGB */
    size_t width, height; /* In pixels, not bytes */
} Image;

/*----------------------------------------------------------------------------*/

/*
 * Initialize an 'Image' structure. The caller is responsible of deinitializing
 * the image with 'image_deinit'.
 */
bool image_init(Image* image, size_t width, size_t height);

/*
 * Free all members of an Image structure. Doesn't free the Image itself.
 */
void image_deinit(Image* image);

/*
 * Group the data of a linear image into squares of side N. If the image
 * dimensions are not divisible by N, they will be increased.
 */
void image_transform_squares(Image* image, size_t square_side);

/*
 * Write the specified 'Image' structure into a PNG file with the specified
 * name, using 'zoom' for scaling.
 */
void image2png(Image* image, const char* filename, int zoom);

#endif /* IMAGE_H_ */
