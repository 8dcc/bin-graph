
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/main.h" /* ByteArray */
#include "include/image.h"
#include "include/util.h"

/*----------------------------------------------------------------------------*/
/* TODO: Read and overwrite at runtime via program arguments. */

/* Width in pixels of the output image (before applying the zoom) */
uint32_t g_output_width = 512;

/* Width and height of each "pixel" when drawn in the actual PNG image */
uint32_t g_output_zoom = 2;

/* Sample size in bytes, used when reading the input. In other words, each pixel
 * in the output will represent `g_sample_step' input bytes. */
uint32_t g_sample_step = 1;

/* If true, each pixel represents the average of `g_sample_step' bytes, instead
 * of just the first byte. */
bool g_average_sample = false;

/*----------------------------------------------------------------------------*/

/* Return the byte samples of a binary file in a linear way */
static ByteArray get_samples(FILE* fp) {
    const size_t file_sz = get_file_size(fp);

    ByteArray result;

    /* Calculate the number of samples we will use for generating the image */
    result.size = file_sz / g_sample_step;
    if (file_sz % g_sample_step != 0)
        result.size++;

    /* Allocate the array for the samples */
    result.data = malloc(result.size);

    for (size_t i = 0; i < result.size; i++) {
        /* Will hold just the first byte or the average, depending on
         * `g_average_sample'. */
        uint64_t byte = fgetc(fp);

        for (uint32_t j = 1;
             j < g_sample_step && (i * g_sample_step) + j < file_sz; j++) {
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
    if (argc != 3)
        die("Usage: %s <input> <output.png>", argv[0]);

    const char* input_filename  = argv[1];
    const char* output_filename = argv[2];

    /* Open the input for reading */
    FILE* fp = fopen(input_filename, "rb");
    if (!fp)
        die("Can't open file: \"%s\"", input_filename);

    /* Store the byte samples in the same order as the file */
    ByteArray samples = get_samples(fp);
    fclose(fp);

    /* TODO: Modify samples depending on mode: b&w, zigzag, z-order, etc. */

    /* Write the actual samples to the PNG image. The colors are decided by the
     * `byte_to_color' static function, inside `image.c'. */
    image_write_png(output_filename, samples);

    /* We are done with the samples */
    free(samples.data);
    return 0;
}
