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

#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#include "include/export.h"
#include "include/image.h"
#include "include/byte_array.h"
#include "include/util.h"

/* Bytes per pixel of the PNG image (R, G, B) */
#define PNG_BPP 3

bool export_png(const Args* args, const Image* image, FILE* output_fp) {
    png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL) {
        ERR("Can't create 'png_structp'.");
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (info == NULL) {
        ERR("Can't create 'png_infop'.");
        return false;
    }

    /* The actual PNG image dimensions, remember that the Image is unscaled */
    assert(image->height > 0 && image->width > 0);
    const int zoom          = args->output_zoom;
    const size_t png_height = image->height * zoom;
    const size_t png_width  = image->width * zoom;

    /* Specify the PNG info */
    png_init_io(png, output_fp);
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
     * Allocate the PNG rows. Since 'png_bytep' is typedef'd to a pointer, this
     * is a (void**).
     */
    png_bytep* rows = calloc(png_height, sizeof(png_bytep));
    if (rows == NULL) {
        ERR("Failed to allocate PNG rows array.");
        return false;
    }

    for (size_t y = 0; y < png_height; y++) {
        rows[y] = malloc(png_width * PNG_BPP);
        if (rows[y] == NULL) {
            ERR("Failed to allocate PNG row #%zu.", y);
            return false;
        }
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
            const Color color = image->pixels[image->width * y + x];

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

    png_destroy_write_struct(&png, &info);

    return true;
}
