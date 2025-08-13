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

Image* generate_dotplot(const Args* args, ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;
    image_init(image, args, bytes->size);

    // FIXME
    assert(image->width == bytes->size && image->height == bytes->size);

    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            Color* color = &image->pixels[image->width * y + x];

            /*
             * The dotplot is used to meassure self-similarity. For each point
             * (X,Y), set the point if the X-th sample matches the Y-th sample.
             *
             * For example:
             *
             *     ABCABD
             *    +------
             *   A|*  *
             *   B| *  *
             *   C|  *
             *   A|*  *
             *   B| *  *
             *   D|     *
             */
            assert(x < bytes->size && y < bytes->size);
            color->r = color->g = color->b =
              (bytes->data[x] == bytes->data[y]) ? 0xFF : 0x00;
        }
    }

    return image;
}
