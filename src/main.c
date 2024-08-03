
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "include/main.h" /* ByteArray */
#include "include/args.h"
#include "include/image.h"
#include "include/util.h"

/*----------------------------------------------------------------------------*/

/* Program mode. Determines how the bytes will be displayed. */
enum EProgramMode g_mode = MODE_ASCII_LINEAR;

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

/* Side of each square used when transforming the generated image. Values lower
 * than two are ignored. */
uint32_t g_transform_squares_side = 0;

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

    /* Perform different transformations to the generated image */
    if (g_transform_squares_side > 1)
        image_transform_squares(&image, g_transform_squares_side);

    /* We are done with the initial samples, free the bytes allocated in
     * `get_samples'. */
    free(samples.data);

    /* Write the Image structure to the PNG file */
    image2png(image, output_filename);

    /* We are done with the image, free the pixels allocated in `image_*' */
    free(image.pixels);

    return 0;
}
