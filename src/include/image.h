
#ifndef IMAGE_H_
#define IMAGE_H_ 1

#include <stdint.h>
#include "read_file.h" /* ByteArray */

typedef struct Color {
    uint8_t r, g, b;
} Color;

typedef struct Image {
    Color* pixels;          /* RGB */
    uint32_t width, height; /* In pixels, not bytes */
} Image;

/*----------------------------------------------------------------------------*/

/*
 * Initialize an Image structure. Get its dimensions, and allocate the pixel
 * array. It must be freed with `image_free'.
 */
void image_init(Image* image, size_t data_sz);

/*
 * Free all members of an Image structure. Doesn't free the Image itself.
 */
void image_free(Image* image);

/*
 * Convert a ByteArray to an Image structure that represents different
 * information of the bytes.
 *
 * Depending on the function, the Image dimensions might change, but normally
 * the width will be `g_output_width' pixels, declared in `main.h'.
 *
 * For more information on each mode, run the program with the `--help'
 * argument.
 */
void image_grayscale(Image* image, ByteArray* bytes);
void image_ascii(Image* image, ByteArray* bytes);
void image_entropy(Image* image, ByteArray* bytes);
void image_histogram(Image* image, ByteArray* bytes);
void image_bigrams(Image* image, ByteArray* bytes);
void image_dotplot(Image* image, ByteArray* bytes);

/*
 * Group the data of a linear image into squares of side N. If the image
 * dimensions are not divisible by N, they will be increased.
 */
void image_transform_squares(Image* image, uint32_t square_side);

/*
 * Write the specified Image structure into a PNG file with the specified name.
 * This function is responsible for scaling the Image depending on
 * `g_output_zoom'.
 */
void image2png(Image* image, const char* filename);

#endif /* IMAGE_H_ */
