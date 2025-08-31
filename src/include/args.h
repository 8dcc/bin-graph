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

#ifndef ARGS_DEFAULT_BLOCK_SIZE
#define ARGS_DEFAULT_BLOCK_SIZE 256
#endif /* ARGS_DEFAULT_BLOCK_SIZE */

#ifndef ARGS_DEFAULT_OUTPUT_WIDTH
#define ARGS_DEFAULT_OUTPUT_WIDTH 512
#endif /* ARGS_DEFAULT_OUTPUT_WIDTH */

#ifndef ARGS_DEFAULT_OUTPUT_ZOOM
#define ARGS_DEFAULT_OUTPUT_ZOOM 2
#endif /* ARGS_DEFAULT_OUTPUT_ZOOM */

/* TODO: Add more modes: bigram_freq, etc. */
enum EArgsMode {
    ARGS_MODE_GRAYSCALE,
    ARGS_MODE_ASCII,
    ARGS_MODE_ENTROPY,
    ARGS_MODE_ENTROPY_HISTOGRAM,
    ARGS_MODE_HISTOGRAM,
    ARGS_MODE_BIGRAMS,
    ARGS_MODE_DOTPLOT,
};

enum EArgsOutputFormat {
    ARGS_OUTPUT_FORMAT_PNG,
    ARGS_OUTPUT_FORMAT_ESC_TEXT,
};

/*----------------------------------------------------------------------------*/

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

    /* Block size used in some modes like 'ARGS_MODE_ENTROPY' */
    size_t block_size;

    /* Output format */
    enum EArgsOutputFormat output_format;

    /* Width in pixels of the output image (before applying the zoom) */
    int output_width;

    /* Start and end offsets for reading the input file. Zero means ignore. */
    size_t offset_start, offset_end;

    /* Width and height of each "pixel" when drawn in the actual PNG image */
    int output_zoom;

    /*
     * Side of each square used when transforming the generated image. Values
     * lower than two are ignored.
     */
    int transform_squares_side;
} Args;

/*----------------------------------------------------------------------------*/

/*
 * Initialize an 'args_t' structure.
 */
void args_init(Args* args);

/*
 * Parse the arguments in the 'argv' array, of length 'argc', storing the
 * results in the 'args_t' structure pointed to by 'args'.
 */
void args_parse(Args* args, int argc, char** argv);

/*
 * Get the name of the specified mode enumerator.
 */
const char* args_get_mode_name(enum EArgsMode mode);

/*
 * Get the name of the specified output format enumerator.
 */
const char* args_get_output_format_name(enum EArgsOutputFormat format);

#endif /* ARGS_H_ */
