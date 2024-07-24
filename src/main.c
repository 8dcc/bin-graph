
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "include/main.h" /* ByteArray */
#include "include/image.h"
#include "include/util.h"

/*----------------------------------------------------------------------------*/

enum EArgError {
    ARG_ERR_NONE  = 0,
    ARG_ERR_EXIT  = 1,
    ARG_ERR_USAGE = 2,
    ARG_ERR_HELP  = 3,
};

/* TODO: Add more modes: zigzag, z-order, bigram_freq, histogram, etc. */
enum EProgramMode {
    MODE_GRAYSCALE,
    MODE_ASCII_LINEAR,
    MODE_ENTROPY,
    MODE_HISTOGRAM,
    MODE_BIGRAMS,
    MODE_DOTPLOT,
};

#define DEFAULT_SAMPLE_STEP  1
#define DEFAULT_BLOCK_SIZE   256
#define DEFAULT_OUTPUT_WIDTH 512
#define DEFAULT_OUTPUT_ZOOM  2

/*----------------------------------------------------------------------------*/

/* Program mode. Determines how the bytes will be displayed. */
enum EProgramMode g_mode = MODE_ASCII_LINEAR;

/* Mode names and descriptions used when parsing the program arguments */
struct {
    const char* arg;
    const char* desc;
} g_mode_names[] = {
    [MODE_GRAYSCALE] = {
        .arg  = "grayscale",
        .desc =
        "          The brightness of each pixel represents the value of each\n"
        "          sample (00..FF).",
    },
    [MODE_ASCII_LINEAR] = {
        .arg  = "ascii_linear",
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

/* Start and end offsets for reading the input file. Zero means ignore. */
size_t g_offset_start = 0;
size_t g_offset_end   = 0;

/* Sample size in bytes, used when reading the input. In other words, each pixel
 * in the output will represent one out of `g_sample_step' input bytes. */
uint32_t g_sample_step = DEFAULT_SAMPLE_STEP;

/* Block size used in some modes like MODE_ENTROPY. */
uint32_t g_block_size = DEFAULT_BLOCK_SIZE;

/* Width in pixels of the output image (before applying the zoom) */
uint32_t g_output_width = DEFAULT_OUTPUT_WIDTH;

/* Width and height of each "pixel" when drawn in the actual PNG image */
uint32_t g_output_zoom = DEFAULT_OUTPUT_ZOOM;

/*----------------------------------------------------------------------------*/

static void parse_args(int argc, char** argv) {
    enum EArgError arg_error = ARG_ERR_NONE;

    if (argc < 3) {
        /* Too few arguments, but one of them was --help */
        if (argc > 1 && (strcmp(argv[argc - 1], "--help") == 0 ||
                         strcmp(argv[argc - 1], "-h") == 0)) {
            arg_error = ARG_ERR_HELP;
            goto check_arg_err;
        }

        log_err("Not enough arguments.");
        arg_error = ARG_ERR_USAGE;
        goto check_arg_err;
    }

    /* Parse arguments */
    for (int i = 1; i < argc - 2; i++) {
        if (argv[i][0] != '-' || argv[i][1] != '-') {
            log_err("Expected option argument, instead found: \"%s\"", argv[i]);
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
                log_err("Not enough arguments for option: \"%s\".", option);
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
                log_err("Unknown mode: \"%s\".\n", argv[i]);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "offset-start") == 0) {
            i++;
            if (i >= argc - 2) {
                log_err("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            if (sscanf(argv[i], "%zx", &g_offset_start) != 1) {
                log_err("Invalid format for start offset. Example: \"e1c5\"");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "offset-end") == 0) {
            i++;
            if (i >= argc - 2) {
                log_err("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            if (sscanf(argv[i], "%zx", &g_offset_end) != 1) {
                log_err("Invalid format for end offset. Example: \"e1c5\"");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "zoom") == 0) {
            i++;
            if (i >= argc - 2) {
                log_err("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_zoom;
            if (sscanf(argv[i], "%d", &signed_zoom) != 1 || signed_zoom <= 0) {
                log_err(
                  "The zoom factor must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_output_zoom = signed_zoom;
        } else if (strcmp(option, "width") == 0) {
            i++;
            if (i >= argc - 2) {
                log_err("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_width;
            if (sscanf(argv[i], "%d", &signed_width) != 1 ||
                signed_width <= 0) {
                log_err("The width must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_output_width = signed_width;
        } else if (strcmp(option, "sample-step") == 0) {
            i++;
            if (i >= argc - 2) {
                log_err("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_step;
            if (sscanf(argv[i], "%d", &signed_step) != 1 || signed_step <= 0) {
                log_err(
                  "The sample step must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_sample_step = signed_step;
        } else if (strcmp(option, "block-size") == 0) {
            i++;
            if (i >= argc - 2) {
                log_err("Not enough arguments for option: \"%s\".", option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            int signed_size;
            if (sscanf(argv[i], "%d", &signed_size) != 1 || signed_size <= 0) {
                log_err("The block size must be an integer greater than zero.");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            g_block_size = signed_size;
        } else {
            log_err("Invalid option: \"%s\".", option);
            arg_error = ARG_ERR_USAGE;
            goto check_arg_err;
        }
    }

    /* Ensure that there are no invalid argument combinations */
    if (g_offset_end != 0 && g_offset_end <= g_offset_start) {
        log_err("The end offset (%zx) must be bigger than the start offset "
                "(%zx).",
                g_offset_end, g_offset_start);
        arg_error = ARG_ERR_EXIT;
        goto check_arg_err;
    }

    /* Also check for ignored argument combinations. These just cause a
     * warning. */
    if (g_output_width != DEFAULT_OUTPUT_WIDTH &&
        (g_mode == MODE_BIGRAMS || g_mode == MODE_DOTPLOT)) {
        log_wrn("The output width will be overwritten by the current mode "
                "(%s).",
                g_mode_names[g_mode].arg);
    }

    if (g_block_size != DEFAULT_BLOCK_SIZE && g_mode != MODE_ENTROPY) {
        log_wrn("The current mode (%s) isn't affected by the block size.",
                g_mode_names[g_mode].arg);
    }

    if (g_block_size <= 1 && g_mode == MODE_ENTROPY) {
        log_wrn("The block size (%d) is too small for this mode (%s). "
                "Overwritting to %d bytes.",
                g_sample_step, g_mode_names[g_mode].arg, DEFAULT_BLOCK_SIZE);
        g_sample_step = DEFAULT_BLOCK_SIZE;
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
                  "  --mode MODE\n"
                  "      Set the current mode to MODE. Available modes:\n");

                for (size_t mode = 0; mode < LENGTH(g_mode_names); mode++)
                    fprintf(stderr,
                            "        %s:\n"
                            "%s\n",
                            g_mode_names[mode].arg, g_mode_names[mode].desc);
            }
        }

        exit(1);
    }
}

/*----------------------------------------------------------------------------*/

/* Return the byte samples of a binary file in a linear way */
static ByteArray get_samples(FILE* fp) {
    const size_t file_sz = get_file_size(fp);

    /* Make sure the offsets are not out of bounds */
    if (g_offset_start > file_sz || g_offset_end > file_sz)
        die("An offset (%zX, %zX) was bigger than the file size (%zX).",
            g_offset_start, g_offset_end, file_sz);

    /* If the global offset is zero, read until the end of the file */
    const size_t offset_end = (g_offset_end == 0) ? file_sz : g_offset_end;

    if (offset_end <= g_offset_start)
        die("End offset (%zX) was smaller or equal than the start offset "
            "(%zX).",
            offset_end, g_offset_start);

    /* Move to the starting offset */
    if (fseek(fp, g_offset_start, SEEK_SET) != 0)
        die("fseek() failed with offset 0x%zX. Errno: %d.", g_offset_start,
            errno);

    /* The size of the section that we are trying to read */
    size_t input_sz = offset_end - g_offset_start;

    /* Calculate the number of samples we will use for generating the image */
    ByteArray result;
    result.size = input_sz / g_sample_step;
    if (input_sz % g_sample_step != 0)
        result.size++;

    /* Allocate the array for the samples */
    result.data = malloc(result.size);
    if (result.data == NULL)
        die("Failed to allocate the samples array (%zu bytes).", result.size);

    for (size_t i = 0; i < result.size; i++) {
        /* Store the first byte of the chunk */
        const int byte = fgetc(fp);

        /* Consume the rest of the chunk */
        for (uint32_t j = 1;
             j < g_sample_step && (i * g_sample_step) + j < input_sz; j++)
            fgetc(fp);

        assert(byte <= 0xFF);
        result.data[i] = byte;
    }

    return result;
}

/*----------------------------------------------------------------------------*/

int main(int argc, char** argv) {
    /* Change the global variables depending on the program arguments */
    parse_args(argc, argv);

    const char* input_filename  = argv[argc - 2];
    const char* output_filename = argv[argc - 1];

    /* Open the input for reading */
    FILE* fp = fopen(input_filename, "rb");
    if (!fp)
        die("Can't open file: \"%s\"", input_filename);

    /* Store the byte samples in the same order as the file */
    ByteArray samples = get_samples(fp);
    fclose(fp);

    /* Convert the samples ByteArray to a different color Image depending on the
     * global mode. */
    Image image;
    switch (g_mode) {
        case MODE_GRAYSCALE:
            image = image_grayscale(samples);
            break;

        case MODE_ASCII_LINEAR:
            image = image_ascii_linear(samples);
            break;

        case MODE_ENTROPY:
            image = image_entropy(samples);
            break;

        case MODE_HISTOGRAM:
            image = image_histogram(samples);
            break;

        case MODE_BIGRAMS:
            image = image_bigrams(samples);
            break;

        case MODE_DOTPLOT:
            image = image_dotplot(samples);
            break;
    }

    /* We are done with the initial samples, free the bytes allocated in
     * `get_samples'. */
    free(samples.data);

    /* Write the Image structure to the PNG file */
    image2png(output_filename, image);

    /* We are done with the image, free the pixels allocated in `image_*' */
    free(image.pixels);

    return 0;
}
