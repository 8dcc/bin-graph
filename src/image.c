
#include <stdlib.h>
#include <png.h>
#include <ctype.h>

#include "include/image.h"
#include "include/main.h" /* ByteArray */
#include "include/util.h"

/*----------------------------------------------------------------------------*/

/* Bytes per pixel of the PNG image (R, G, B) */
#define PNG_BPP 3

/*----------------------------------------------------------------------------*/

Image image_ascii_linear(ByteArray bytes) {
    /* The image conversion functions ignore zoom. It will be applied when
     * generating the PNG. */
    Image image;
    image.width  = g_output_width;
    image.height = bytes.size / image.width;
    if (bytes.size % image.width != 0)
        image.height++;

    /* Allocate the array that will contain the color information */
    image.pixels = malloc(image.height * image.width * sizeof(Color));

    for (uint32_t y = 0; y < image.height; y++) {
        for (uint32_t x = 0; x < image.width; x++) {
            /* One-dimensional index for both the `bytes.data' and
             * `image.pixels' arrays. */
            const size_t raw_idx = image.width * y + x;

            /* Pointer to the current color in the Image */
            Color* color = &image.pixels[raw_idx];

            if (raw_idx >= bytes.size) {
                /* If we are not in-bounds, we are filling the last row; use a
                 * generic padding color. */
                color->r = 0;
                color->g = 0;
                color->b = 0;
                continue;
            }

            /* Determine the RGB color of the pixel depending on the byte
             * value. */
            const uint8_t byte = bytes.data[raw_idx];
            if (byte == 0x00 || byte == 0xFF) {
                /* Common padding values, either black or white */
                color->r = byte;
                color->g = byte;
                color->b = byte;
            } else if (isgraph(byte) || isspace(byte)) {
                /* Printable ASCII, blue */
                color->r = 0x37;
                color->g = 0x7E;
                color->b = 0xB8;
            } else {
                /* Unknown, red */
                color->r = 0xE4;
                color->g = 0x1A;
                color->b = 0x1C;
            }
        }
    }

    return image;
}

/*----------------------------------------------------------------------------*/

void image2png(const char* filename, Image image) {
    FILE* fd = fopen(filename, "wb");
    if (!fd)
        die("Can't open file: \"%s\"\n", filename);

    png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        die("Can't create png_structp\n");

    png_infop info = png_create_info_struct(png);
    if (!info)
        die("Can't create png_infop\n");

    /* The actual PNG image dimensions, remember that the Image is unscaled */
    const int zoom      = g_output_zoom;
    uint32_t png_height = image.height * zoom;
    uint32_t png_width  = image.width * zoom;

    /* Specify the PNG info */
    png_init_io(png, fd);
    png_set_IHDR(png, info, png_width, png_height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    /* Allocate the PNG rows. Since png_bytep is typedef'd to a pointer, this is
     * a (void**). */
    png_bytep* rows = malloc(png_height * sizeof(png_bytep));
    for (uint32_t y = 0; y < png_height; y++)
        rows[y] = malloc(png_width * PNG_BPP);

    /* Write the `bytes' array we received into the `rows' array we just
     * allocated.
     *
     * The outer loops iterate the unscaled pixels, and are needed for accessing
     * the `bytes.data' array. */
    for (uint32_t y = 0; y < image.height; y++) {
        for (uint32_t x = 0; x < image.width; x++) {
            Color color = image.pixels[image.width * y + x];

            /* Draw a rectangle of side `g_output_zoom' */
            for (int rect_y = 0; rect_y < zoom; rect_y++) {
                for (int rect_x = 0; rect_x < zoom; rect_x++) {
                    const png_bytep row = rows[zoom * y + rect_y];

                    /* Note that we are using RGB, not RGBA */
                    row[PNG_BPP * (zoom * x + rect_x)]     = color.r;
                    row[PNG_BPP * (zoom * x + rect_x) + 1] = color.g;
                    row[PNG_BPP * (zoom * x + rect_x) + 2] = color.b;
                }
            }
        }
    }

    /* Write the rows into the PNG structure */
    png_write_image(png, rows);
    png_write_end(png, NULL);

    /* Free each pointer of the `rows' array, and the array itself */
    for (uint32_t y = 0; y < png_height; y++)
        free(rows[y]);
    free(rows);

    fclose(fd);
    png_destroy_write_struct(&png, &info);
}

/*----------------------------------------------------------------------------*/

void image_free(Image* image) {
    free(image->pixels);
    image->pixels = NULL;
}
