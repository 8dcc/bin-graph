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

#ifndef ARGS_H_
#define ARGS_H_ 1

#include <stddef.h>
#include <stdint.h>

enum EArgsError {
    ARGS_ERR_NONE  = 0,
    ARGS_ERR_EXIT  = 1,
    ARGS_ERR_USAGE = 2,
    ARGS_ERR_HELP  = 3,
};

/* TODO: Add more modes: bigram_freq, etc. */
enum EArgsMode {
    ARGS_MODE_GRAYSCALE,
    ARGS_MODE_ASCII,
    ARGS_MODE_ENTROPY,
    ARGS_MODE_HISTOGRAM,
    ARGS_MODE_BIGRAMS,
    ARGS_MODE_DOTPLOT,
};

/*
 * Structure filled by 'args_parse' to indicate the program's command-line
 * arguments.
 */
typedef struct args {
    /* Input and output filenames */
    const char* input_filename;
    const char* output_filename;

    /* Program mode. Determines how the bytes will be displayed. */
    enum EArgsMode mode;

    /* Block size used in some modes like MODE_ENTROPY. */
    uint32_t block_size;

    /* Width in pixels of the output image (before applying the zoom) */
    uint32_t output_width;

    /* Start and end offsets for reading the input file. Zero means ignore. */
    size_t offset_start, offset_end;

    /* Width and height of each "pixel" when drawn in the actual PNG image */
    uint32_t output_zoom;

    /*
     * Side of each square used when transforming the generated image. Values
     * lower than two are ignored.
     */
    uint32_t transform_squares_side;
} args_t;

/*----------------------------------------------------------------------------*/

/*
 * Initialize an 'args_t' structure.
 */
void args_init(args_t* args);

/*
 * Parse the arguments in the 'argv' array, of length 'argc', storing the
 * results in the 'args_t' structure pointed to by 'args'.
 */
void args_parse(args_t* args, int argc, char** argv);

#endif /* ARGS_H_ */
