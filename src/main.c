
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

enum EArgError {
    ARG_ERR_NONE  = 0,
    ARG_ERR_EXIT  = 1,
    ARG_ERR_USAGE = 2,
    ARG_ERR_HELP  = 3,
};

/* TODO: Add more modes: zigzag, z-order, bigram_freq, etc. */
enum EProgramMode {
    MODE_GRAYSCALE,
    MODE_ASCII_LINEAR,
    MODE_BIGRAMS,
    MODE_DOTPLOT,
};

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
        "          The color of each pixel represents the 'printability' of\n"
        "          each sample in a linear way. Black represents a null byte\n"
        "          (0x00), white represents a set byte (0xFF), blue\n"
        "          represents printable characters and red represents any\n"
        "          other value.",
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
        "          Meassure self-similarity. A point (X,Y) in the graph shows\n"
        "          if the X-th sample matches the Y-th sample.",
    },
};

/* Start and end offsets for reading the input file. Zero means ignore. */
size_t g_offset_start = 0;
size_t g_offset_end   = 0;

/* Sample size in bytes, used when reading the input. In other words, each pixel
 * in the output will represent `g_sample_step' input bytes. */
uint32_t g_sample_step = 1;

/* If true, each pixel represents the average of `g_sample_step' bytes, instead
 * of just the first byte. */
bool g_average_sample = false;

/* Width in pixels of the output image (before applying the zoom) */
uint32_t g_output_width = 512;

/* Width and height of each "pixel" when drawn in the actual PNG image */
uint32_t g_output_zoom = 2;

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

        fprintf(stderr, "Not enough arguments.\n");
        arg_error = ARG_ERR_USAGE;
        goto check_arg_err;
    }

    /* Parse arguments */
    for (int i = 1; i < argc - 2; i++) {
        if (argv[i][0] != '-' || argv[i][1] != '-') {
            fprintf(stderr, "Expected option argument, instead found: \"%s\"\n",
                    argv[i]);
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
                fprintf(stderr, "Not enough arguments for option: \"%s\".\n",
                        option);
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
                fprintf(stderr, "Unknown mode: \"%s\".\n", argv[i]);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else if (strcmp(option, "offsets") == 0) {
            i++;
            if (i >= argc - 2) {
                fprintf(stderr, "Not enough arguments for option: \"%s\".\n",
                        option);
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            if (sscanf(argv[i], "%lx-%lx", &g_offset_start, &g_offset_end) !=
                2) {
                fprintf(stderr,
                        "Invalid format for start and end offsets. Example: "
                        "\"e1c5-ff10\"\n");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }

            if (g_offset_end <= g_offset_start) {
                fprintf(stderr, "The end offset must be bigger than the start "
                                "offset.\n");
                arg_error = ARG_ERR_EXIT;
                goto check_arg_err;
            }
        } else {
            fprintf(stderr, "Invalid option: \"%s\".\n", option);
            arg_error = ARG_ERR_USAGE;
            goto check_arg_err;
        }
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
                  "  --offsets START-END\n"
                  "      Only process from START to END of file. Specified "
                  "in hexadecimal\n"
                  "      format, without any prefix.\n\n"
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

    /* The actual samples that we plan on reading */
    size_t input_sz = file_sz;

    /* Check if we just want to read a section of the file */
    if (g_offset_end > 0) {
        if (g_offset_end <= g_offset_start)
            die("End offset (%lX) was smaller or equal than the start offset "
                "(%lX).",
                g_offset_end, g_offset_start);

        if (g_offset_start > file_sz || g_offset_end > file_sz)
            die("Tried reading an offset bigger than the file size.\n"
                "Offsets: %lX-%lX. File size: %lX.",
                g_offset_start, g_offset_end, file_sz);

        /* Move to the starting offset */
        if (fseek(fp, g_offset_start, SEEK_SET) != 0)
            die("fseek() failed with offset 0x%lX. Errno: %d.", g_offset_start,
                errno);

        /* The size of the section that we are trying to read */
        input_sz = g_offset_end - g_offset_start;
    }

    ByteArray result;

    /* Calculate the number of samples we will use for generating the image */
    result.size = input_sz / g_sample_step;
    if (input_sz % g_sample_step != 0)
        result.size++;

    /* Allocate the array for the samples */
    result.data = malloc(result.size);

    for (size_t i = 0; i < result.size; i++) {
        /* Will hold just the first byte or the average, depending on
         * `g_average_sample'. */
        uint64_t byte = fgetc(fp);

        for (uint32_t j = 1;
             j < g_sample_step && (i * g_sample_step) + j < input_sz; j++) {
            /* Consume the rest of the chunk, and accumulate them if we want to
             * average them later. */
            const int next_byte = fgetc(fp);

            if (g_average_sample)
                byte += next_byte;
        }

        /* If we accumulated the other bytes, average them */
        if (g_average_sample)
            byte /= g_sample_step;

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
