
#ifndef IMAGE_H_
#define IMAGE_H_ 1

#include <stdint.h>
#include "main.h" /* ByteArray */

typedef struct Color {
    uint8_t r, g, b;
} Color;

/*----------------------------------------------------------------------------*/

/* Write the specified `bytes' array into a PNG file called `filename'.
 * Each pixel row will be `g_output_width' pixels wide, declared in `main.h'. */
void image_write_png(const char* filename, ByteArray bytes);

#endif /* IMAGE_H_ */
