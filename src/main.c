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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/image.h"
#include "include/generate.h"
#include "include/util.h"

int main(int argc, char** argv) {
    Args args;
    args_init(&args);
    args_parse(&args, argc, argv);

    /* Open the input for reading */
    FILE* fp = fopen(args.input_filename, "rb");
    if (fp == NULL)
        DIE("Can't open file '%s': %s", args.input_filename, strerror(errno));

    /* Allocate the bytes needed for reading this chunk of the file */
    ByteArray file_bytes;
    byte_array_init(&file_bytes, fp, args.offset_start, args.offset_end);

    /* Read and store the file bytes in a linear way */
    read_file(&file_bytes, fp, args.offset_start, args.offset_end);
    fclose(fp);

    /*
     * Convert the ByteArray to a color Image depending on the global mode.
     *
     * TODO: Since all these functions share the same interface, we could set a
     * function pointer variable and call that, instead of duplicating this
     * calling logic.
     */
    Image* image;
    switch (args.mode) {
        case ARGS_MODE_GRAYSCALE:
            image = generate_grayscale(&args, &file_bytes);
            break;

        case ARGS_MODE_ASCII:
            image = generate_ascii(&args, &file_bytes);
            break;

        case ARGS_MODE_ENTROPY:
            image = generate_entropy(&args, &file_bytes);
            break;

        case ARGS_MODE_HISTOGRAM:
            image = generate_histogram(&args, &file_bytes);
            break;

        case ARGS_MODE_BIGRAMS:
            image = generate_bigrams(&args, &file_bytes);
            break;

        case ARGS_MODE_DOTPLOT:
            image = generate_dotplot(&args, &file_bytes);
            break;
    }

    /* We are done with the initial file bytes, free them */
    byte_array_free(&file_bytes);

    /* Perform different transformations to the generated image */
    if (args.transform_squares_side > 1)
        image_transform_squares(image, args.transform_squares_side);

    /* Write the Image structure to the PNG file */
    image2png(image, args.output_filename, args.output_zoom);

    /* We are done with the image, free it */
    image_free(image);
    free(image);

    return 0;
}
