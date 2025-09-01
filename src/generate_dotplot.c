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
#include "include/byte_array.h"
#include "include/util.h"

static bool validate_args(const Args* args) {
    if (args->block_size != ARGS_DEFAULT_BLOCK_SIZE)
        WRN("The current mode (%s) is not affected by the user-specified block "
            "size (%zu).",
            args_get_mode_name(args->mode),
            args->block_size);
    if (args->output_width != ARGS_DEFAULT_OUTPUT_WIDTH)
        WRN("The user-specified output width (%d) will be overwritten by the "
            "current mode (%s).",
            args->output_width,
            args_get_mode_name(args->mode));
    return true;
}

static inline Image* alloc_and_init_image(ByteArray* bytes) {
    Image* image = malloc(sizeof(Image));
    if (image == NULL)
        return NULL;

    const size_t width  = bytes->size;
    const size_t height = bytes->size;
    if (!image_init(image, width, height))
        return NULL;

    return image;
}

/*----------------------------------------------------------------------------*/

Image* generate_dotplot(const Args* args, ByteArray* bytes) {
    if (!validate_args(args))
        return NULL;

    Image* image = alloc_and_init_image(bytes);
    if (image == NULL)
        return NULL;
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
