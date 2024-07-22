
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
 * This list briefly explains what each function is meant to represent:
 *   - image_grayscale: The brightness of each pixel represents the value of
 *     each byte (00..FF).
 *   - image_ascii_linear: The color of each pixel represents the "printability"
 *     of each byte in a linear way. Black represents a null byte (0x00), white
 *     represents a set byte (0xFF), blue represents printable characters and
 *     red represents any other value.
 *   - histogram: Each row represents a byte (00..FF), and the width of each
 *     line represents the frequency of that byte relative to the most frequent
 *     one.
 *   - image_bigrams: The coordinates of each point are determined by a pair of
 *     samples in the input. This can be used to identify patterns of different
 *     file formats.
 *   - dotplot: Meassure self-similarity. A point (X,Y) in the graph shows if
 *     the X-th sample matches the Y-th sample. For more information, see the
 *     definition of the function.
 */
Image image_grayscale(ByteArray bytes);
Image image_ascii_linear(ByteArray bytes);
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
