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

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <argp.h>

#include "include/args.h"
#include "include/util.h"

/*----------------------------------------------------------------------------*/

#ifndef DEFAULT_BLOCK_SIZE
#define DEFAULT_BLOCK_SIZE 256
#endif /* DEFAULT_BLOCK_SIZE */

#ifndef DEFAULT_OUTPUT_WIDTH
#define DEFAULT_OUTPUT_WIDTH 512
#endif /* DEFAULT_OUTPUT_WIDTH */

#ifndef DEFAULT_OUTPUT_ZOOM
#define DEFAULT_OUTPUT_ZOOM 2
#endif /* DEFAULT_OUTPUT_ZOOM */

#ifndef VERSION
#define VERSION NULL
#endif /* VERSION */

/*
 * Used when building the 'argp' structure in 'args_parse'.
 */
#define ARGS_DOC    "INPUT OUTPUT"
#define PROGRAM_DOC "Simple program for visualizing binary files."

/*
 * IDs for command-line arguments that don't have a short version
 * (e.g. '--print-interfaces', etc.).
 */
enum ELongOptionIds {
    LONGOPT_OFFSET_START = 256,
    LONGOPT_OFFSET_END,
    LONGOPT_BLOCK_SIZE,
    LONGOPT_TRANSFORM_SQUARES,
    LONGOPT_LIST_MODES,
};

/*
 * Globals from 'argp.h'.
 */
const char* argp_program_version     = VERSION;
const char* argp_program_bug_address = "<8dcc.git@gmail.com>";

/*----------------------------------------------------------------------------*/

/*
 * Mode names and descriptions used when parsing the program arguments.
 */
static struct {
    const char* arg;
    const char* desc;
} g_mode_names[] = {
    [ARGS_MODE_GRAYSCALE] = {
      .arg  = "grayscale",
      .desc = "The brightness of each pixel represents the value of each "
              "sample (00..FF).",
    },
    [ARGS_MODE_ASCII] = {
      .arg  = "ascii",
      .desc = "The color of each pixel represents the \"printability\" of each "
              "sample in a linear way. Black represents a null byte (00), "
              "white represents a set byte (FF), blue represents printable "
              "characters and red represents any other value.",
    },
    [ARGS_MODE_ENTROPY] = {
      .arg  = "entropy",
      .desc = "The intensity of each pixel represents its entropy (i.e. its "
              "\"predictability\"). This is useful for distinguishing "
              "compressed/encrypted from non-compressed chunks.",
    },
    [ARGS_MODE_HISTOGRAM] = {
      .arg  = "histogram",
      .desc = "Each row represents a byte (00..FF), and the width of each line "
              "represents the frequency of that byte relative to the most "
              "frequent one.",
    },
    [ARGS_MODE_BIGRAMS] = {
      .arg  = "bigrams",
      .desc = "The coordinates of each point are determined by a pair of "
              "samples in the input. This can be used to identify patterns of "
              "different file formats.",
    },
    [ARGS_MODE_DOTPLOT] = {
      .arg  = "dotplot",
      .desc = "Measure self-similarity. A point (X,Y) in the graph shows if "
              "the X-th sample matches the Y-th sample.",
    },
};

/*
 * Command-line options used by the Argp library.
 */
static struct argp_option options[] = {
    { NULL, 0, NULL, 0, "General options", 1 },
    {
      "mode",
      'm',
      "MODE",
      0,
      "Set the current mode to MODE. Use `--list-modes' for a list of modes.",
      1,
    },
    { NULL, 0, NULL, 0, "Input options", 2 },
    {
      "offset-start",
      LONGOPT_OFFSET_START,
      "OFFSET",
      0,
      "Start processing the file from OFFSET. Specified in hexadecimal format, "
      "without any prefix.",
      2,
    },
    {
      "offset-end",
      LONGOPT_OFFSET_END,
      "OFFSET",
      0,
      "Stop processing the file at OFFSET. Specified in hexadecimal format, "
      "without any prefix. Zero means the end of the file.",
      2,
    },
    {
      "block-size",
      LONGOPT_BLOCK_SIZE,
      "BYTES",
      0,
      "Set the size for some block-specific modes like entropy.",
      2,
    },
    { NULL, 0, NULL, 0, "Output options", 3 },
    {
      "zoom",
      'z',
      "FACTOR",
      0,
      "Scale each pixel by FACTOR.",
      3,
    },
    {
      "width",
      'w',
      "WIDTH",
      0,
      "Set the default width to WIDTH. This parameter is ignored in some modes "
      "(bigrams, dotplot, etc.). This width indicates the sample number in "
      "each row before applying the zoom.",
      3,
    },
    {
      "transform-squares",
      LONGOPT_TRANSFORM_SQUARES,
      "SIDE",
      0,
      "After generating the image, group its pixels into squares of side SIDE. "
      "If the image dimensions are not divisible by SIDE, they will be "
      "increased. This option is useful with the entropy mode.",
      3,
    },
    { NULL, 0, NULL, 0, "Help options", 4 },
    {
      "list-modes",
      LONGOPT_LIST_MODES,
      NULL,
      0,
      "List the availiable values for the `--mode' option, along with their "
      "descriptions.",
      4,
    },
    { NULL, 0, NULL, 0, NULL, 0 }
};

/*----------------------------------------------------------------------------*/

/*
 * Write the corresponding mode enumerator from its name. The function returns
 * true on success, or false if the provided name does not match any known mode.
 */
static bool mode_name_to_enumerator(const char* name, enum EArgsMode* out) {
    /*
     * NOTE: This assumes that the mode enumerators start from zero, and are
     * sequencial.
     */
    for (size_t mode = 0; mode < LENGTH(g_mode_names); mode++) {
        if (strcmp(name, g_mode_names[mode].arg) == 0) {
            *out = mode;
            return true;
        }
    }

    return false;
}

/*
 * Callback function used by the Argp library (specifically, by 'argp_parse'
 * through the 'argp' structure) for parsing each option in the command-line
 * arguments.
 */
static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    /*
     * Get the 'input' argument from 'argp_parse', which we know is a pointer to
     * our 'Args' structure.
     */
    args_t* parsed_args = state->input;

    switch (key) {
        case 'm': {
            if (!mode_name_to_enumerator(arg, &parsed_args->mode)) {
                fprintf(state->err_stream,
                        "%s: Unknown mode '%s'\n",
                        state->name,
                        arg);
                argp_usage(state);
            }
        } break;

        case 'z': {
            int signed_zoom;
            if (sscanf(arg, "%d", &signed_zoom) != 1 || signed_zoom <= 0) {
                fprintf(state->err_stream,
                        "%s: The zoom factor must be an integer greater than "
                        "zero.\n",
                        state->name);
                argp_usage(state);
            }
            parsed_args->output_zoom = signed_zoom;
        } break;

        case 'w': {
            int signed_width;
            if (sscanf(arg, "%d", &signed_width) != 1 || signed_width <= 0) {
                fprintf(state->err_stream,
                        "%s: The width must be an integer greater than zero.\n",
                        state->name);
                argp_usage(state);
            }
            parsed_args->output_width = signed_width;
        } break;

        case LONGOPT_OFFSET_START: {
            if (sscanf(arg, "%zx", &parsed_args->offset_start) != 1) {
                fprintf(state->err_stream,
                        "%s: Invalid format for start offset. Example: "
                        "\"e1c5\".\n",
                        state->name);
                argp_usage(state);
            }
        } break;

        case LONGOPT_OFFSET_END: {
            if (sscanf(arg, "%zx", &parsed_args->offset_end) != 1) {
                fprintf(state->err_stream,
                        "%s: Invalid format for end offset. Example: "
                        "\"e1c5\".\n",
                        state->name);
                argp_usage(state);
            }
        } break;

        case LONGOPT_BLOCK_SIZE: {
            int signed_size;
            if (sscanf(arg, "%d", &signed_size) != 1 || signed_size <= 0) {
                fprintf(state->err_stream,
                        "%s: The block size must be an integer greater than "
                        "zero.\n",
                        state->name);
                argp_usage(state);
            }
            parsed_args->block_size = signed_size;
        } break;

        case LONGOPT_TRANSFORM_SQUARES: {
            int signed_side;
            if (sscanf(arg, "%d", &signed_side) != 1 || signed_side <= 0) {
                fprintf(state->err_stream,
                        "%s: The square side must be an integer greater than "
                        "zero.\n",
                        state->name);
                argp_usage(state);
            }
            parsed_args->transform_squares_side = signed_side;
        } break;

        case LONGOPT_LIST_MODES: {
            /* TODO: Wrap descriptions to column 80 when priting */
            for (size_t mode = 0; mode < LENGTH(g_mode_names); mode++) {
                printf("* %s: %s\n",
                       g_mode_names[mode].arg,
                       g_mode_names[mode].desc);
            }
            exit(0);
        } break;

        case ARGP_KEY_ARG: {
            if (state->arg_num >= 2) {
                fprintf(state->err_stream,
                        "%s: Too many arguments.\n",
                        state->name);
                argp_usage(state);
            }

            if (state->arg_num == 0)
                parsed_args->input_filename = arg;
            else
                parsed_args->output_filename = arg;
        } break;

        case ARGP_KEY_END: {
            if (state->arg_num < 2) {
                fprintf(state->err_stream,
                        "%s: Not enough arguments.\n",
                        state->name);
                argp_usage(state);
            }
        } break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/

void args_init(args_t* args) {
    args->input_filename         = NULL;
    args->output_filename        = NULL;
    args->mode                   = ARGS_MODE_ASCII;
    args->block_size             = DEFAULT_BLOCK_SIZE;
    args->output_width           = DEFAULT_OUTPUT_WIDTH;
    args->offset_start           = 0;
    args->offset_end             = 0;
    args->output_zoom            = DEFAULT_OUTPUT_ZOOM;
    args->transform_squares_side = 0;
}

void args_parse(args_t* args, int argc, char** argv) {
    static struct argp argp = {
        options, parse_opt, ARGS_DOC, PROGRAM_DOC, NULL, NULL, NULL,
    };

    argp_parse(&argp, argc, argv, 0, 0, args);

    assert(args->input_filename != NULL);
    assert(args->output_filename != NULL);
}
