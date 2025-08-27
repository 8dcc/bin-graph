
#include <stdbool.h>
#include <stdio.h>

#include "include/export.h"
#include "include/image.h"
#include "include/util.h"

/*
 * Print an empty ASCII character with the specified RGB background to the
 * specified file.
 */
static void print_ascii_color(FILE* fp, Color color, int zoom) {
    /* Set the background character */
    fprintf(fp, "\033[48;2;%d;%d;%dm", color.r, color.g, color.b);

    /* Print N empty characters, with a different background color */
    for (int i = 0; i < zoom; i++)
        fputc(' ', fp);

    /* Reset the color for future calls */
    fputs("\033[0m", fp);
}

/*----------------------------------------------------------------------------*/

bool export_escaped_text(const Args* args, const Image* image, FILE* output_fp) {
    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            const Color color = image->pixels[image->width * y + x];
            print_ascii_color(output_fp, color, args->output_zoom);
        }
        fputc('\n', output_fp);
    }

    return true;
}
