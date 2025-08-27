
#include <stdbool.h>
#include <stdio.h>

#include "include/export.h"
#include "include/image.h"
#include "include/util.h"

static bool validate_args(const Args* args) {
    if (args->output_zoom != ARGS_DEFAULT_OUTPUT_ZOOM)
        WRN("The current output format (%s) is not affected zoom (%d).",
            args_get_output_format_name(args->output_format),
            args->output_zoom);
    return true;
}

/*
 * Print an empty ASCII character with the specified RGB background to the
 * specified file.
 */
static void print_ascii_color(FILE* fp, Color color) {
    /* Set the background character */
    fprintf(fp, "\033[48;2;%d;%d;%dm", color.r, color.g, color.b);

    /* Print an empty character, with a different background color */
    fputc(' ', fp);

    /* Reset the color for future calls */
    fputs("\033[0m", fp);
}

/*----------------------------------------------------------------------------*/

bool export_ascii(const Args* args, const Image* image, FILE* output_fp) {
    validate_args(args);

    for (size_t y = 0; y < image->height; y++) {
        for (size_t x = 0; x < image->width; x++) {
            const Color color = image->pixels[image->width * y + x];
            print_ascii_color(output_fp, color);
        }
        fputc('\n', output_fp);
    }

    return true;
}
