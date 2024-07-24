
#ifndef IMAGE_H_
#define IMAGE_H_ 1

#include <stdint.h>
#include "main.h" /* ByteArray */

typedef struct Color {
    uint8_t r, g, b;
} Color;

typedef struct Image {
    Color* pixels;          /* RGB */
    uint32_t width, height; /* In pixels, not bytes */
} Image;

/*----------------------------------------------------------------------------*/

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
Image image_grayscale(ByteArray bytes);
Image image_ascii_linear(ByteArray bytes);
Image image_entropy(ByteArray bytes);
Image image_histogram(ByteArray bytes);
Image image_bigrams(ByteArray bytes);
Image image_dotplot(ByteArray bytes);

/* Write the specified Image structure into a PNG file with the specified name.
 * This function is responsible for scaling the Image depending on
 * `g_output_zoom'. */
void image2png(const char* filename, Image image);

/* Free all the members of an Image structure. Doesn't free the Image itself. */
void image_free(Image* image);

#endif /* IMAGE_H_ */
