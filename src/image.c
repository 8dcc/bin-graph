
#include <stdlib.h>
#include <png.h>

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
    result.r = byte;
    result.g = byte;
    result.b = byte;
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

    uint32_t image_height = bytes.size / g_output_width;
    if (bytes.size % g_output_width != 0)
        image_height++;

    /* Specify the PNG info */
    png_init_io(png, fd);
    png_set_IHDR(png, info, g_output_width, image_height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    /* Allocate the PNG rows. Since png_bytep is typedef'd to a pointer, this is
     * a (void**). */
    png_bytep* rows = malloc(image_height * sizeof(png_bytep));
    for (uint32_t y = 0; y < image_height; y++)
        rows[y] = malloc(g_output_width * sizeof(uint8_t) * COL_SZ);

    /* Write the `bytes' array we received into the `rows' array we just
     * allocated. */
    for (uint32_t y = 0; y < image_height; y++) {
        for (uint32_t x = 0; x < g_output_width * COL_SZ; x += COL_SZ) {
            const size_t bytes_pos = g_output_width * y + x;

            /* If we are inside the bytes array, store the corresponding color
             * to this byte. Otherwise, we are filling the last row; just use
             * black. */
            const Color color = (bytes_pos < bytes.size)
                                  ? byte_to_color(bytes.data[bytes_pos])
                                  : COLOR_PADDING;

            /* Note that we are using RGB, not RGBA */
            rows[y][x]     = color.r;
            rows[y][x + 1] = color.g;
            rows[y][x + 2] = color.b;
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
