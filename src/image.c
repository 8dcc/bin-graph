
#include <stdlib.h>
#include <png.h>
#include <ctype.h>

#include "include/args.h"
#include "include/read_file.h"
#include "include/image.h"
#include "include/util.h"

/*----------------------------------------------------------------------------*/

/* Bytes per pixel of the PNG image (R, G, B) */
#define PNG_BPP 3

/*----------------------------------------------------------------------------*/

Image image_grayscale(ByteArray bytes) {
    /* The image conversion functions ignore zoom. It will be applied when
     * generating the PNG. */
    Image image;
    image.width  = g_output_width;
    image.height = bytes.size / image.width;
    if (bytes.size % image.width != 0)
        image.height++;

    /* Allocate the array that will contain the color information. We need to
     * cast the first value to `size_t' to make sure the multiplication result
     * doesn't overflow in an `uint32_t'. */
    image.pixels = calloc((size_t)image.height * image.width, sizeof(Color));
    if (image.pixels == NULL)
        die("Failed to allocate pixel array.");

    for (size_t y = 0; y < image.height; y++) {
        for (size_t x = 0; x < image.width; x++) {
            /* One-dimensional index for both the `bytes.data' and
             * `image.pixels' arrays. */
            const size_t raw_idx = image.width * y + x;

            /* Pointer to the current color in the Image */
            Color* color = &image.pixels[raw_idx];

            /* The color brightness is determined by the byte value */
            color->r = color->g = color->b = bytes.data[raw_idx];
        }
    }

    return image;
}

Image image_ascii_linear(ByteArray bytes) {
    Image image;
    image.width  = g_output_width;
    image.height = bytes.size / image.width;
    if (bytes.size % image.width != 0)
        image.height++;

    image.pixels = calloc((size_t)image.height * image.width, sizeof(Color));
    if (image.pixels == NULL)
        die("Failed to allocate pixel array.");

    for (size_t y = 0; y < image.height; y++) {
        for (size_t x = 0; x < image.width; x++) {
            const size_t raw_idx = (size_t)image.width * y + x;
            Color* color         = &image.pixels[raw_idx];

            /* If we are not in-bounds, we are filling the last row; use a
             * generic padding color. */
            if (raw_idx >= bytes.size) {
                color->r = color->g = color->b = 0;
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

Image image_entropy(ByteArray bytes) {
    Image image;
    image.width  = g_output_width;
    image.height = bytes.size / image.width;
    if (bytes.size % image.width != 0)
        image.height++;

    image.pixels = calloc((size_t)image.height * image.width, sizeof(Color));
    if (image.pixels == NULL)
        die("Failed to allocate pixel array.");

    /* Iterate blocks of the input, each will share the same entropy color */
    for (size_t i = 0; i < bytes.size; i += g_block_size) {
        /* Make sure we are not reading past the end of `bytes.size' */
        const size_t real_block_size =
          (i + g_block_size < bytes.size) ? g_block_size : bytes.size - i;

        /* Calculate the Shannon entropy for this block */
        const double block_entropy = entropy(&bytes.data[i], real_block_size);

        /* Calculate the [00..FF] color for this block based on the [0..8]
         * entropy. */
        uint8_t color_intensity = block_entropy * 255 / 8;

        /* Render this block with the same color */
        for (size_t j = 0; j < real_block_size; j++) {
            Color* color = &image.pixels[i + j];
            color->r = color->g = color->b = color_intensity;
        }
    }

    return image;
}

Image image_histogram(ByteArray bytes) {
    Image image;
    image.width  = g_output_width;
    image.height = 256;

    image.pixels = calloc((size_t)image.height * image.width, sizeof(Color));
    if (image.pixels == NULL)
        die("Failed to allocate pixel array.");

    uint8_t most_frequent = 0;
    uint32_t* occurrences = calloc(image.height, sizeof(uint32_t));
    if (occurrences == NULL)
        die("Failed to allocate occurrences array.");

    /* Store the occurrences including the most frequent byte */
    for (size_t i = 0; i < bytes.size; i++) {
        const uint8_t byte = bytes.data[i];

        occurrences[byte]++;

        if (occurrences[byte] > occurrences[most_frequent])
            most_frequent = byte;
    }

    /* Draw each horizontal line based on occurrences relative to the most
     * frequen byte. */
    for (size_t y = 0; y < image.height; y++) {
        const uint32_t line_width =
          occurrences[y] * image.width / occurrences[most_frequent];

        for (size_t x = 0; x < line_width; x++) {
            Color* color = &image.pixels[image.width * y + x];
            color->r = color->g = color->b = 0xFF;
        }
    }

    /* We are done with the occurrences array */
    free(occurrences);

    return image;
}

Image image_bigrams(ByteArray bytes) {
    Image image;
    image.width  = 256;
    image.height = 256;

    image.pixels = calloc((size_t)image.height * image.width, sizeof(Color));
    if (image.pixels == NULL)
        die("Failed to allocate pixel array.");

    /* Initialize the image to black */
    for (size_t y = 0; y < image.height; y++) {
        for (size_t x = 0; x < image.width; x++) {
            Color* color = &image.pixels[image.width * y + x];
            color->r = color->g = color->b = 0x00;
        }
    }

    /* In this case we don't want to iterate the image, but the bytes. We start
     * from the second byte because we are plotting bigrams (pairs). */
    for (size_t i = 1; i < bytes.size; i++) {
        const uint8_t x = bytes.data[i - 1];
        const uint8_t y = bytes.data[i];

        /* The position is determined by the values of the current byte and the
         * previous one. */
        Color* color = &image.pixels[image.width * y + x];

        /* This mode just plots whether a bigram is present or not in the input.
         * We don't change the colors depending on the occurrences or anything
         * like that. */
        color->r = color->g = color->b = 0xFF;
    }

    return image;
}

Image image_dotplot(ByteArray bytes) {
    Image image;
    image.width  = bytes.size;
    image.height = bytes.size;

    image.pixels = calloc((size_t)image.height * image.width, sizeof(Color));
    if (image.pixels == NULL)
        die("Failed to allocate pixel array.");

    for (size_t y = 0; y < image.height; y++) {
        for (size_t x = 0; x < image.width; x++) {
            Color* color = &image.pixels[image.width * y + x];

            /*
             * The dotplot is used to meassure self-similarity. For each point
             * (X,Y), set the point if the X-th sample matches the Y-th sample.
             *
             * For example:
             *
             *     ABCABD
             *    +------
             *   A|*  *
             *   B| *  *
             *   C|  *
             *   A|*  *
             *   B| *  *
             *   D|     *
             */
            color->r = color->g = color->b =
              (bytes.data[x] == bytes.data[y]) ? 0xFF : 0x00;
        }
    }

    return image;
}

/*----------------------------------------------------------------------------*/

void image_transform_squares(Image* image, uint32_t square_side) {
    const int square_size     = square_side * square_side;
    const size_t total_pixels = image->width * image->height;

    /* Increase the width and height if they are not divisible by the square
     * side. */
    if (image->width % square_side != 0)
        image->width += square_side - image->width % square_side;
    if (image->height % square_side != 0)
        image->height += square_side - image->height % square_side;

    /* Number of squares in each row. Division should be exact now. */
    const size_t squares_per_row = image->width / square_side;

    /* Allocate the array with the new image dimensions */
    Color* new_pixels =
      calloc((size_t)image->height * image->width, sizeof(Color));

    /* Iterate the original pixels */
    for (size_t i = 0; i < total_pixels; i++) {
        /* Number and coordinates of the current square */
        const size_t square_num = i / square_size;
        const size_t square_y   = square_num / squares_per_row;
        const size_t square_x   = square_num % squares_per_row;

        /* Internal number and coordinates inside the current square */
        const size_t internal_num = i % square_size;
        const size_t internal_y   = internal_num / square_side;
        const size_t internal_x   = internal_num % square_side;

        /* Final coordinates in the image for the new point */
        const size_t final_y = square_side * square_y + internal_y;
        const size_t final_x = square_side * square_x + internal_x;

        /* Copy the color in the old position to the new one */
        new_pixels[image->width * final_y + final_x] = image->pixels[i];
    }

    /* Free the old pixel array and overwrite the pointer with the new one */
    free(image->pixels);
    image->pixels = new_pixels;
}

/*----------------------------------------------------------------------------*/

void image2png(Image image, const char* filename) {
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
    png_bytep* rows = calloc(png_height, sizeof(png_bytep));
    if (rows == NULL)
        die("Failed to allocate PNG rows.");

    for (uint32_t y = 0; y < png_height; y++) {
        rows[y] = malloc(png_width * PNG_BPP);
        if (rows[y] == NULL)
            die("Failed to allocate PNG row %d.", y);
    }

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
