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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/image.h"
#include "include/util.h"

int main(int argc, char** argv) {
    /*
     * Change the global variables (declared in args.h) depending on the program
     * arguments.
     */
    parse_args(argc, argv);

    const char* input_filename  = argv[argc - 2];
    const char* output_filename = argv[argc - 1];

    /* Open the input for reading */
    FILE* fp = fopen(input_filename, "rb");
    if (!fp)
        DIE("Can't open file: \"%s\"", input_filename);

    /* Allocate the bytes needed for reading this chunk of the file */
    ByteArray file_bytes;
    byte_array_init(&file_bytes, fp, g_offset_start, g_offset_end);

    /* Read and store the file bytes in a linear way */
    read_file(&file_bytes, fp, g_offset_start, g_offset_end);
    fclose(fp);

    /* Initialize the image structure */
    Image image;
    image_init(&image, file_bytes.size);

    /* Convert the ByteArray to a color Image depending on the global mode */
    switch (g_mode) {
        case MODE_GRAYSCALE:
            image_grayscale(&image, &file_bytes);
            break;

        case MODE_ASCII:
            image_ascii(&image, &file_bytes);
            break;

        case MODE_ENTROPY:
            image_entropy(&image, &file_bytes);
            break;

        case MODE_HISTOGRAM:
            image_histogram(&image, &file_bytes);
            break;

        case MODE_BIGRAMS:
            image_bigrams(&image, &file_bytes);
            break;

        case MODE_DOTPLOT:
            image_dotplot(&image, &file_bytes);
            break;
    }

    /* We are done with the initial file bytes, free them */
    byte_array_free(&file_bytes);

    /* Perform different transformations to the generated image */
    if (g_transform_squares_side > 1)
        image_transform_squares(&image, g_transform_squares_side);

    /* Write the Image structure to the PNG file */
    image2png(&image, output_filename);

    /* We are done with the image, free it */
    image_free(&image);

    return 0;
}
