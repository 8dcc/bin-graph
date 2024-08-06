
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/image.h"
#include "include/util.h"

int main(int argc, char** argv) {
    /* Change the global variables (declared in args.h) depending on the program
     * arguments. */
    parse_args(argc, argv);

    const char* input_filename  = argv[argc - 2];
    const char* output_filename = argv[argc - 1];

    /* Open the input for reading */
    FILE* fp = fopen(input_filename, "rb");
    if (!fp)
        die("Can't open file: \"%s\"", input_filename);

    /* Read and store the file bytes in a linear way */
    ByteArray file_bytes = read_file(fp, g_offset_start, g_offset_end);
    fclose(fp);

    /* Convert the ByteArray to a color Image depending on the global mode */
    Image image;
    switch (g_mode) {
        case MODE_GRAYSCALE:
            image = image_grayscale(file_bytes);
            break;

        case MODE_ASCII_LINEAR:
            image = image_ascii_linear(file_bytes);
            break;

        case MODE_ENTROPY:
            image = image_entropy(file_bytes);
            break;

        case MODE_HISTOGRAM:
            image = image_histogram(file_bytes);
            break;

        case MODE_BIGRAMS:
            image = image_bigrams(file_bytes);
            break;

        case MODE_DOTPLOT:
            image = image_dotplot(file_bytes);
            break;
    }

    /* Perform different transformations to the generated image */
    if (g_transform_squares_side > 1)
        image_transform_squares(&image, g_transform_squares_side);

    /* We are done with the initial file bytes, free the data allocated in
     * `get_file_bytes'. */
    free(file_bytes.data);

    /* Write the Image structure to the PNG file */
    image2png(image, output_filename);

    /* We are done with the image, free the pixels allocated in `image_*' */
    free(image.pixels);

    return 0;
}
