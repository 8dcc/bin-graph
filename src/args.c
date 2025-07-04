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
#include <string.h>
#include <stdlib.h>

#include "include/args.h"
#include "include/util.h"

/*----------------------------------------------------------------------------*/

/* Program mode. Determines how the bytes will be displayed. */
enum EProgramMode g_mode = MODE_ASCII;

/* Start and end offsets for reading the input file. Zero means ignore. */
size_t g_offset_start = 0;
size_t g_offset_end   = 0;

/* Block size used in some modes like MODE_ENTROPY. */
uint32_t g_block_size = DEFAULT_BLOCK_SIZE;

/* Width in pixels of the output image (before applying the zoom) */
uint32_t g_output_width = DEFAULT_OUTPUT_WIDTH;

/* Width and height of each "pixel" when drawn in the actual PNG image */
uint32_t g_output_zoom = DEFAULT_OUTPUT_ZOOM;

/*
 * Side of each square used when transforming the generated image. Values lower
 * than two are ignored.
 */
uint32_t g_transform_squares_side = 0;

/*----------------------------------------------------------------------------*/

/* Mode names and descriptions used when parsing the program arguments */
static struct {
    const char* arg;
    const char* desc;
} g_mode_names[] = {
    [MODE_GRAYSCALE] = {
        .arg  = "grayscale",
        .desc =
        "          The brightness of each pixel represents the value of each\n"
        "          sample (00..FF).",
    },
    [MODE_ASCII] = {
        .arg  = "ascii",
        .desc =
        "          The color of each pixel represents the \"printability\" of\n"
        "          each sample in a linear way. Black represents a null byte\n"
        "          (00), white represents a set byte (FF), blue represents\n"
        "          printable characters and red represents any other value.",
    },
    [MODE_ENTROPY] = {
        .arg  = "entropy",
        .desc =
        "          The intensity of each pixel represents its entropy (i.e.\n"
        "          its \"predictability\"). This is useful for distinguishing\n"
        "          compressed/encrypted from non-compressed chunks.",
    },
    [MODE_HISTOGRAM] = {
        .arg  = "histogram",
        .desc =
        "          Each row represents a byte (00..FF), and the width of each\n"
        "          line represents the frequency of that byte relative to the\n"
        "          most frequent one.",
    },
    [MODE_BIGRAMS] = {
        .arg  = "bigrams",
        .desc =
        "          The coordinates of each point are determined by a pair of\n"
        "          samples in the input. This can be used to identify\n"
        "          patterns of different file formats.",
    },
    [MODE_DOTPLOT] = {
        .arg  = "dotplot",
        .desc =
        "          Measure self-similarity. A point (X,Y) in the graph shows\n"
        "          if the X-th sample matches the Y-th sample.",
    },
};

/*----------------------------------------------------------------------------*/

void parse_args(int argc, char** argv) {
    enum EArgError arg_error = ARG_ERR_NONE;

    if (argc < 3) {
        /* Too few arguments, but one of them was --help */
        if (argc > 1 && (strcmp(argv[argc - 1], "--help") == 0 ||
                         strcmp(argv[argc - 1], "-h") == 0)) {
            arg_error = ARG_ERR_HELP;
            goto check_arg_err;
        }

        ERR("Not enough arguments.");
        arg_error = ARG_ERR_USAGE;
        goto check_arg_err;
    }

    /* Parse arguments */
    for (int i = 1; i < argc - 2; i++) {
        if (argv[i][0] != '-' || argv[i][1] != '-') {
            ERR("Expected option argument, instead found: \"%s\"", argv[i]);
            arg_error = ARG_ERR_USAGE;
            goto check_arg_err;
        }

        const char* option = &argv[i][2];
        if (strcmp(option, "help") == 0) {
            arg_error = ARG_ERR_HELP;
            goto check_arg_err;
        } else if (strcmp(option, "mode") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            bool got_match = false;
            for (size_t mode = 0; mode < LENGTH(g_mode_names); mode++) {
                if (strcmp(argv[i], g_mode_names[mode].arg) == 0) {
                    got_match = true;
                    g_mode    = mode;
                    break;
                }
            }

            if (!got_match) {
                ERR("Unknown mode: \"%s\".\n", argv[i]);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "offset-start") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            if (sscanf(argv[i], "%zx", &g_offset_start) != 1) {
                ERR("Invalid format for start offset. Example: \"e1c5\"");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "offset-end") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            if (sscanf(argv[i], "%zx", &g_offset_end) != 1) {
                ERR("Invalid format for end offset. Example: \"e1c5\"");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "zoom") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_zoom;
            if (sscanf(argv[i], "%d", &signed_zoom) != 1 || signed_zoom <= 0) {
                ERR("The zoom factor must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_output_zoom = signed_zoom;
        } else if (strcmp(option, "width") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_width;
            if (sscanf(argv[i], "%d", &signed_width) != 1 ||
                signed_width <= 0) {
                ERR("The width must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_output_width = signed_width;
        } else if (strcmp(option, "block-size") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_size;
            if (sscanf(argv[i], "%d", &signed_size) != 1 || signed_size <= 0) {
                ERR("The block size must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_block_size = signed_size;
        } else if (strcmp(option, "transform-squares") == 0) {
            i++;
            if (i >= argc - 2) {
                ERR("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_side;
            if (sscanf(argv[i], "%d", &signed_side) != 1 || signed_side <= 1) {
                ERR("The square side must be an integer greater than one.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_transform_squares_side = signed_side;
        } else {
            ERR("Invalid option: \"%s\".", option);
            arg_error = ARG_ERR_USAGE;
            goto check_arg_err;
        }
    }

    /* Ensure that there are no invalid argument combinations */
    if (g_offset_end != 0 && g_offset_end <= g_offset_start) {
        ERR("The end offset (%zx) must be bigger than the start offset "
            "(%zx).",
            g_offset_end,
            g_offset_start);
        arg_error = ARG_ERR_EXIT;
        goto check_arg_err;
    }

    /*
     * Also check for ignored argument combinations. These just cause a
     * warning.
     */
    if (g_output_width != DEFAULT_OUTPUT_WIDTH &&
        (g_mode == MODE_BIGRAMS || g_mode == MODE_DOTPLOT)) {
        ERR("Warning: The output width will be overwritten by the current mode "
            "(%s).",
            g_mode_names[g_mode].arg);
    }

    if (g_block_size != DEFAULT_BLOCK_SIZE && g_mode != MODE_ENTROPY) {
        ERR("Warning: The current mode (%s) isn't affected by the block size.",
            g_mode_names[g_mode].arg);
    }

    if (g_block_size <= 1 && g_mode == MODE_ENTROPY) {
        ERR("Warning: The block size (%d) is too small for the current mode "
            "(%s). Overwritting to %d bytes.",
            g_block_size,
            g_mode_names[g_mode].arg,
            DEFAULT_BLOCK_SIZE);
        g_block_size = DEFAULT_BLOCK_SIZE;
    }

    if (g_transform_squares_side > 1 &&
        (g_mode == MODE_HISTOGRAM || g_mode == MODE_BIGRAMS ||
         g_mode == MODE_DOTPLOT)) {
        ERR("Warning: The \"squares\" transformation is not recommended for "
            "the current mode (%s).",
            g_mode_names[g_mode].arg);
    }

check_arg_err:
    if (arg_error >= ARG_ERR_EXIT) {
        if (arg_error >= ARG_ERR_USAGE) {
            fprintf(stderr,
                    "Usage:\n"
                    "  %s [OPTION...] INPUT OUTPUT.png\n",
                    argv[0]);

            if (arg_error == ARG_ERR_HELP) {
                fprintf(
                  stderr,
                  "\nPossible options:\n"
                  "  --help\n"
                  "      Show this help and exit the program.\n\n"
                  "  --offset-start OFFSET\n"
                  "      Start processing the file from OFFSET. Specified in "
                  "hexadecimal\n"
                  "      format, without any prefix.\n\n"
                  "  --offset-end OFFSET\n"
                  "      Stop processing the file at OFFSET. Specified in "
                  "hexadecimal\n"
                  "      format, without any prefix. Zero means the end of the "
                  "file.\n\n"
                  "  --zoom FACTOR\n"
                  "      Scale each pixel by FACTOR.\n\n"
                  "  --width WIDTH\n"
                  "      Set the default width to WIDTH. This parameter is "
                  "ignored in\n"
                  "      some modes (bigrams, dotplot, etc.). This width "
                  "indicates the\n"
                  "      sample number in each row before applying the "
                  "zoom.\n\n"
                  "  --sample-step BYTES\n"
                  "      Only read one out of every BYTES from the input. "
                  "Specified in\n"
                  "      decimal format.\n\n"
                  "  --block-size BYTES\n"
                  "      Set the size for some block-specific modes like "
                  "entropy.\n\n"
                  "  --transform-squares SIDE\n"
                  "      After generating the image, group its pixels into "
                  "squares of\n"
                  "      side SIDE. If the image dimensions are not divisible "
                  "by SIDE,\n"
                  "      they will be increased. This option is useful with "
                  "the entropy\n"
                  "      mode.\n\n"
                  "  --mode MODE\n"
                  "      Set the current mode to MODE. Available modes:\n");

                for (size_t mode = 0; mode < LENGTH(g_mode_names); mode++)
                    fprintf(stderr,
                            "        %s:\n"
                            "%s\n",
                            g_mode_names[mode].arg,
                            g_mode_names[mode].desc);
            }
        }

        exit(1);
    }
}
