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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/image.h"
#include "include/generate.h"
#include "include/export.h"
#include "include/util.h"

int main(int argc, char** argv) {
    Args args;
    args_init(&args);
    args_parse(&args, argc, argv);

    /*
     * Open the input for reading.
     * TODO: Support "-" as a filename for standard input.
     */
    FILE* input_fp = fopen(args.input_filename, "rb");
    if (input_fp == NULL)
        DIE("Can't open file '%s': %s", args.input_filename, strerror(errno));

    /* Read and store the file bytes in a 'ByteArray' */
    ByteArray file_bytes;
    read_file(&file_bytes, input_fp, args.offset_start, args.offset_end);
    fclose(input_fp);

    /* Obtain the image generation function from the program mode */
    generation_func_ptr_t generation_func =
      generation_func_from_mode(args.mode);
    assert(generation_func != NULL);

    /* Convert the ByteArray to a color Image depending on the global mode */
    Image* image = generation_func(&args, &file_bytes);

    /* We are done with the initial file bytes, free them */
    byte_array_destroy(&file_bytes);

    /* Perform different transformations to the generated image */
    if (args.transform_squares_side > 1)
        image_transform_squares(image, args.transform_squares_side);

    /* Open the output file for writing */
    FILE* output_fp = (strcmp(args.output_filename, "-") == 0)
                        ? stdout
                        : fopen(args.output_filename, "wb");
    if (output_fp == NULL)
        DIE("Can't open file '%s': %s", args.output_filename, strerror(errno));

    /* Obtain the export function from the program arguments */
    export_func_ptr_t export_func =
      export_func_from_output_format(args.output_format);
    assert(export_func != NULL);

    /* Export the 'Image' structure to the file */
    export_func(&args, image, output_fp);

    /* We are done with the image, free it */
    image_deinit(image);
    free(image);

    fclose(output_fp);

    return 0;
}
