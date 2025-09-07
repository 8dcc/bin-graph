/*
 * Copyright 2025 8dcc
 *
 * This file is part of bin-graph.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <math.h>

#include "include/transform.h"
#include "include/args.h"
#include "include/image.h"
#include "include/util.h"

/*
 * Enumerator with all possible directions/orientations of a Hilbert curve.
 */
enum EDirection {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
};

/*
 * Structure representing the context needed to draw a Hilbert curve.
 */
typedef struct {
    /* Output image for drawing the transformed image */
    Image* output_image;

    /* Current position in "blocks" or "Hilbert points" in the 'output' image */
    size_t x, y;

    /* Side (not size) of a block (i.e. Hilbert curve point) when drawing */
    size_t block_side;

    /* Input image, of the same size as the output */
    const Image* input_image;

    /* Position in the input buffer, in characters */
    size_t input_pos;
} HilbertCtx;

/*----------------------------------------------------------------------------*/

static bool validate_args(const Args* args) {
    switch (args->mode) {
        case ARGS_MODE_HISTOGRAM:
        case ARGS_MODE_ENTROPY_HISTOGRAM:
        case ARGS_MODE_BIGRAMS:
        case ARGS_MODE_DOTPLOT:
            WRN("The Hilbert curve transformation is not recommended for the "
                "current mode (%s).",
                args_get_mode_name(args->mode));
            break;
        default:
            break;
    }
    return true;
}

/*
 * Draw the next input character into the output grid in the current position of
 * the 'HilbertCtx' structure.
 */
static void draw_next(HilbertCtx* ctx) {
    const size_t base_y = ctx->y * ctx->block_side;
    const size_t base_x = ctx->x * ctx->block_side;
    for (size_t y = 0; y < ctx->block_side; y++) {
        for (size_t x = 0; x < ctx->block_side; x++) {
            const size_t raw_y = base_y + y;
            const size_t raw_x = base_x + x;

            assert(raw_y < ctx->output_image->height);
            assert(raw_x < ctx->output_image->width);

            /*
             * Ensure we are not out-of-bounds in the input, since the output
             * might be bigger after squaring.
             */
            if (ctx->input_pos >=
                ctx->input_image->width * ctx->input_image->height)
                return;

            ctx->output_image
              ->pixels[ctx->output_image->width * raw_y + raw_x] =
              ctx->input_image->pixels[ctx->input_pos++];
        }
    }
}

/*
 * Move the coordinates in the 'HilbertCtx' to the specified direction.
 */
static void move(HilbertCtx* ctx, enum EDirection direction) {
    switch (direction) {
        case DIR_UP:
            assert(ctx->y > 0);
            ctx->y--;
            break;
        case DIR_DOWN:
            assert(ctx->y + 1 < ctx->output_image->height);
            ctx->y++;
            break;
        case DIR_LEFT:
            assert(ctx->x > 0);
            ctx->x--;
            break;
        case DIR_RIGHT:
            assert(ctx->x + 1 < ctx->output_image->width);
            ctx->x++;
            break;
    }
}

/*
 * Generate a hilbert curve with the specified recursion level and orientation
 * in the current position according to the 'HilbertCtx'. The input and output
 * pointers are also obtained from the 'HilbertCtx'.
 */
static void recursive_hilbert(HilbertCtx* ctx,
                              int level,
                              enum EDirection direction) {
    if (level <= 1) {
        /*
         * Last recursive level, draw the simplest form:
         *
         *   o      o
         *   |      |
         *   |      |
         *   o------o
         */
        switch (direction) {
            case DIR_UP:
                draw_next(ctx);
                move(ctx, DIR_DOWN);
                draw_next(ctx);
                move(ctx, DIR_RIGHT);
                draw_next(ctx);
                move(ctx, DIR_UP);
                draw_next(ctx);
                break;
            case DIR_DOWN:
                draw_next(ctx);
                move(ctx, DIR_UP);
                draw_next(ctx);
                move(ctx, DIR_LEFT);
                draw_next(ctx);
                move(ctx, DIR_DOWN);
                draw_next(ctx);
                break;
            case DIR_LEFT:
                draw_next(ctx);
                move(ctx, DIR_RIGHT);
                draw_next(ctx);
                move(ctx, DIR_DOWN);
                draw_next(ctx);
                move(ctx, DIR_LEFT);
                draw_next(ctx);
                break;
            case DIR_RIGHT:
                draw_next(ctx);
                move(ctx, DIR_LEFT);
                draw_next(ctx);
                move(ctx, DIR_UP);
                draw_next(ctx);
                move(ctx, DIR_RIGHT);
                draw_next(ctx);
                break;
        }
    } else {
        /*
         * We are not in the last recursive level; draw the same shape, but
         * calling ourselves recursively each time.
         *
         *   [+]    [+]
         *    |      |
         *    |      |
         *   [+]----[+]
         *
         * Where each [+] represents a smaller Hilbert curve that is drawn with
         * a specific orientation.
         */
        switch (direction) {
            case DIR_UP:
                recursive_hilbert(ctx, level - 1, DIR_LEFT);
                move(ctx, DIR_DOWN);
                recursive_hilbert(ctx, level - 1, DIR_UP);
                move(ctx, DIR_RIGHT);
                recursive_hilbert(ctx, level - 1, DIR_UP);
                move(ctx, DIR_UP);
                recursive_hilbert(ctx, level - 1, DIR_RIGHT);
                break;
            case DIR_DOWN:
                recursive_hilbert(ctx, level - 1, DIR_RIGHT);
                move(ctx, DIR_UP);
                recursive_hilbert(ctx, level - 1, DIR_DOWN);
                move(ctx, DIR_LEFT);
                recursive_hilbert(ctx, level - 1, DIR_DOWN);
                move(ctx, DIR_DOWN);
                recursive_hilbert(ctx, level - 1, DIR_LEFT);
                break;
            case DIR_LEFT:
                recursive_hilbert(ctx, level - 1, DIR_UP);
                move(ctx, DIR_RIGHT);
                recursive_hilbert(ctx, level - 1, DIR_LEFT);
                move(ctx, DIR_DOWN);
                recursive_hilbert(ctx, level - 1, DIR_LEFT);
                move(ctx, DIR_LEFT);
                recursive_hilbert(ctx, level - 1, DIR_DOWN);
                break;
            case DIR_RIGHT:
                recursive_hilbert(ctx, level - 1, DIR_DOWN);
                move(ctx, DIR_LEFT);
                recursive_hilbert(ctx, level - 1, DIR_RIGHT);
                move(ctx, DIR_UP);
                recursive_hilbert(ctx, level - 1, DIR_RIGHT);
                move(ctx, DIR_RIGHT);
                recursive_hilbert(ctx, level - 1, DIR_UP);
                break;
        }
    }
}

bool transform_hilbert(const Args* args, Image* input_image) {
    assert(args->transform_hilbert_level > 0);
    if (!validate_args(args))
        return false;

    /*
     * Ensure the height is divisible by the width, allowing us to stack
     * squares.
     */
    Image output_image = {
        .pixels = NULL,
        .height = input_image->height,
        .width  = input_image->width,
    };
    if (output_image.height % output_image.width != 0)
        output_image.height +=
          output_image.width - (output_image.height % output_image.width);

    /* Number of hilbert points per square side (not in total) */
    const size_t draws_per_side = (size_t)pow(2, args->transform_hilbert_level);
    if (draws_per_side > output_image.width) {
        ERR("Not enough width for the specified hilbert level (expected at "
            "least %zu).",
            draws_per_side);
        return false;
    }

    /*
     * Calculate the width of each hilbert point (block), ensuring the width can
     * be divided into the necessary points.
     */
    if (output_image.width % draws_per_side != 0) {
        ERR("Need to draw %zu hilbert points, but the width is not divisible.",
            draws_per_side);
        return false;
    }
    const size_t block_side = output_image.width / draws_per_side;

    /* Allocate the array with the new image dimensions */
    output_image.pixels =
      calloc(output_image.width * output_image.height, sizeof(Color));
    if (output_image.pixels == NULL) {
        ERR("Failed to allocate new pixels array.");
        return false;
    }

    /*
     * Define the 'HilbertCtx' structure that will be modified each iteration
     * below.
     */
    HilbertCtx ctx = {
        .output_image = &output_image,
        .x            = 0,
        .y            = 0,
        .block_side   = block_side,
        .input_image  = input_image,
        .input_pos    = 0,
    };

    /*
     * Each iteration, a square with a Hilbert curve will be placed in the
     * output image, below the previous one.
     */
    for (size_t i = 0;
         ctx.input_pos < ctx.input_image->width * ctx.input_image->height;
         i++) {
        /*
         * Move the context position one Hilbert square down.
         */
        ctx.y = i * draws_per_side;
        ctx.x = 0;

        /*
         * Generate the actual hilbert curve, starting from the top left and
         * ending on the bottom right, allowing us to stack curves on top of
         * each other:
         *
         *     o------o
         *            |
         *            |
         *     o------o
         */
        recursive_hilbert(&ctx, args->transform_hilbert_level, DIR_LEFT);
    }

    /* Free the old pixel array and overwrite the pointer with the new one */
    free(input_image->pixels);
    input_image->pixels = output_image.pixels;
    input_image->height = output_image.height;
    input_image->width  = output_image.width;

    return true;
}
