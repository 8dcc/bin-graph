
#include <stdlib.h>
#include <png.h>
#include <ctype.h>

#include "include/image.h"
#include "include/main.h" /* ByteArray */
#include "include/util.h"

/*----------------------------------------------------------------------------*/

/* Bytes of each color in the PNG image (R, G, B). */
#define COL_SZ 3

/* Used for filling the last row of the image, if incomplete */
static const Color COLOR_PADDING = { 0, 0, 0 };

/*----------------------------------------------------------------------------*/

/* Function used to determine the RGB color of a pixel in the graph, depending
 * on the byte value. */
static Color byte_to_color(uint8_t byte) {
    Color result;

    /* Common padding values, either black or white */
    if (byte == 0x00 || byte == 0xFF) {
        result.r = byte;
        result.g = byte;
        result.b = byte;
    } else if (isgraph(byte) || isspace(byte)) {
        result.r = 0x37;
        result.g = 0x7E;
        result.b = 0xB8;
    } else {
        result.r = 0xE4;
        result.g = 0x1A;
        result.b = 0x1C;
    }

    return result;
}

void image_write_png(const char* filename, ByteArray bytes) {
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

    uint32_t bitmap_height = bytes.size / g_output_width;
    if (bytes.size % g_output_width != 0)
        bitmap_height++;

    /* The actual PNG image dimensions, with zoom */
    const int zoom        = g_output_zoom;
    uint32_t image_height = bitmap_height * zoom;
    uint32_t image_width  = g_output_width * zoom;

    /* Specify the PNG info */
    png_init_io(png, fd);
    png_set_IHDR(png, info, image_width, image_height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    /* Allocate the PNG rows. Since png_bytep is typedef'd to a pointer, this is
     * a (void**). */
    png_bytep* rows = malloc(image_height * sizeof(png_bytep));
    for (uint32_t y = 0; y < image_height; y++)
        rows[y] = malloc(image_width * COL_SZ * sizeof(uint8_t));

    /* Write the `bytes' array we received into the `rows' array we just
     * allocated.
     *
     * The outer loops iterate the unscaled pixels, and are needed for accessing
     * the `bytes.data' array. */
    for (uint32_t y = 0; y < bitmap_height; y++) {
        for (uint32_t x = 0; x < g_output_width; x++) {
            const size_t bytes_pos = g_output_width * y + x;

            /* If we are inside the bytes array, store the corresponding color
             * to this byte. Otherwise, we are filling the last row; use a
             * generic padding color. */
            const Color color = (bytes_pos < bytes.size)
                                  ? byte_to_color(bytes.data[bytes_pos])
                                  : COLOR_PADDING;

            /* Draw a rectangle of side `g_output_zoom`. */
            for (int rect_y = 0; rect_y < zoom; rect_y++) {
                for (int rect_x = 0; rect_x < zoom; rect_x++) {
                    const png_bytep row = rows[zoom * y + rect_y];

                    /* Note that we are using RGB, not RGBA */
                    row[COL_SZ * ((zoom * x) + rect_x)]     = color.r;
                    row[COL_SZ * ((zoom * x) + rect_x) + 1] = color.g;
                    row[COL_SZ * ((zoom * x) + rect_x) + 2] = color.b;
                }
            }
        }
    }

    /* Write the rows into the PNG structure */
    png_write_image(png, rows);
    png_write_end(png, NULL);

    /* Free each pointer of the `rows' array, and the array itself */
    for (uint32_t y = 0; y < image_height; y++)
        free(rows[y]);
    free(rows);

    fclose(fd);
    png_destroy_write_struct(&png, &info);
}
