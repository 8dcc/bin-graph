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
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <png.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/image.h"
#include "include/util.h"

/* Bytes per pixel of the PNG image (R, G, B) */
#define PNG_BPP 3

/*----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------*/

void image_transform_squares(Image* image, size_t square_side) {
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

/*----------------------------------------------------------------------------*/

void image2png(Image* image, const char* filename, int zoom) {
    FILE* fd = fopen(filename, "wb");
    if (fd == NULL)
        DIE("Can't open file '%s': %s", filename, strerror(errno));

    png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
        DIE("Can't create 'png_structp'. Aborting.");

    png_infop info = png_create_info_struct(png);
    if (info == NULL)
        DIE("Can't create 'png_infop'. Aborting.");

    /* The actual PNG image dimensions, remember that the Image is unscaled */
    const size_t png_height = image->height * zoom;
    const size_t png_width  = image->width * zoom;

    /* Specify the PNG info */
    png_init_io(png, fd);
    png_set_IHDR(png,
                 info,
                 png_width,
                 png_height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    /*
     * Allocate the PNG rows. Since png_bytep is typedef'd to a pointer, this is
     * a (void**).
     */
    png_bytep* rows = calloc(png_height, sizeof(png_bytep));
    if (rows == NULL)
        DIE("Failed to allocate PNG rows");

    for (size_t y = 0; y < png_height; y++) {
        rows[y] = malloc(png_width * PNG_BPP);
        if (rows[y] == NULL)
            DIE("Failed to allocate PNG row %zu", y);
    }

    /*
     * Write the 'bytes' array we received into the 'rows' array we just
     * allocated.
     *
     * The outer loops iterate the unscaled pixels, and are needed for accessing
     * the 'bytes->data' array.
     */
    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            Color color = image->pixels[image->width * y + x];

            /* Draw a rectangle of side 'zoom' */
            for (int rect_y = 0; rect_y < zoom; rect_y++) {
                for (int rect_x = 0; rect_x < zoom; rect_x++) {
                    const png_bytep row = rows[zoom * y + rect_y];

                    /* Note that we are using RGB, not RGBA */
                    row[PNG_BPP * (zoom * x + rect_x)]     = color.r;
                    row[PNG_BPP * (zoom * x + rect_x) + 1] = color.g;
                    row[PNG_BPP * (zoom * x + rect_x) + 2] = color.b;
                }
            }
        }
    }

    /* Write the rows into the PNG structure */
    png_write_image(png, rows);
    png_write_end(png, NULL);

    /* Free each pointer of the 'rows' array, and the array itself */
    for (size_t y = 0; y < png_height; y++)
        free(rows[y]);
    free(rows);

    fclose(fd);
    png_destroy_write_struct(&png, &info);
}
