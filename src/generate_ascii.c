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
#include <ctype.h>

#include "include/generate.h"
#include "include/image.h"
#include "include/args.h"
#include "include/read_file.h"

Image* generate_ascii(const Args* args, ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;
    image_init(image, args, bytes->size);

    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            const size_t raw_idx = (size_t)image->width * y + x;
            Color* color         = &image->pixels[raw_idx];

            /*
             * If we are not in-bounds, we are filling the last row; use a
             * generic padding color.
             */
            if (raw_idx >= bytes->size) {
                color->r = color->g = color->b = 0x00;
                continue;
            }

            /*
             * Determine the RGB color of the pixel depending on the byte
             * value.
             */
            const uint8_t byte = bytes->data[raw_idx];
            if (byte == 0x00 || byte == 0xFF) {
                /* Common padding values, either black or white */
                color->r = byte;
                color->g = byte;
                color->b = byte;
            } else if (isgraph(byte) || isspace(byte)) {
                /* Printable ASCII, blue */
                color->r = 0x37;
                color->g = 0x7E;
                color->b = 0xB8;
            } else {
                /* Unknown, red */
                color->r = 0xE4;
                color->g = 0x1A;
                color->b = 0x1C;
            }
        }
    }

    return image;
}
