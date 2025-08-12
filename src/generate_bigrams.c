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

Image* generate_bigrams(const Args* args, ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;
    image_init(image, args, bytes->size);

    // FIXME
    assert(image->width == 256 && image->height == 256);

    /* Initialize the image to black */
    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            Color* color = &image->pixels[image->width * y + x];
            color->r = color->g = color->b = 0x00;
        }
    }

    /*
     * In this case we don't want to iterate the image, but the bytes-> We start
     * from the second byte because we are plotting bigrams (pairs).
     */
    for (size_t i = 1; i < bytes->size; i++) {
        const uint8_t x = bytes->data[i - 1];
        const uint8_t y = bytes->data[i];

        /*
         * The position is determined by the values of the current byte and the
         * previous one.
         */
        Color* color = &image->pixels[image->width * y + x];

        /*
         * This mode just plots whether a bigram is present or not in the input.
         * We don't change the colors depending on the occurrences or anything
         * like that.
         */
        color->r = color->g = color->b = 0xFF;
    }

    return image;
}
