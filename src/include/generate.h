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

#ifndef GENERATE_H_
#define GENERATE_H_ 1

#include "args.h"       /* Args */
#include "byte_array.h" /* ByteArray */
#include "image.h"      /* Image */

/*
 * Pointer to a function that generates an 'Image' from a 'ByteArray'.
 */
typedef Image* (*generation_func_ptr_t)(const Args* args, ByteArray* bytes);

/*
 * From the specified 'ByteArray' structure, generate an 'Image' that represents
 * different information about the input bytes.
 *
 * Depending on the function, the 'Image' dimensions might change, but normally
 * the width will match the 'output_width' member of the received 'Args'
 * structure.
 *
 * For more information on each mode, run the program with the '--list-modes'
 * argument.
 */
Image* generate_grayscale(const Args* args, ByteArray* bytes);
Image* generate_ascii(const Args* args, ByteArray* bytes);
Image* generate_entropy(const Args* args, ByteArray* bytes);
Image* generate_entropy_histogram(const Args* args, ByteArray* bytes);
Image* generate_histogram(const Args* args, ByteArray* bytes);
Image* generate_bigrams(const Args* args, ByteArray* bytes);
Image* generate_dotplot(const Args* args, ByteArray* bytes);

/*----------------------------------------------------------------------------*/

/*
 * Return a pointer to the generation function associated to a specific mode.
 */
static inline generation_func_ptr_t generation_func_from_mode(
  enum EArgsMode mode) {
    switch (mode) {
        case ARGS_MODE_GRAYSCALE:
            return generate_grayscale;
        case ARGS_MODE_ASCII:
            return generate_ascii;
        case ARGS_MODE_ENTROPY:
            return generate_entropy;
        case ARGS_MODE_ENTROPY_HISTOGRAM:
            return generate_entropy_histogram;
        case ARGS_MODE_HISTOGRAM:
            return generate_histogram;
        case ARGS_MODE_BIGRAMS:
            return generate_bigrams;
        case ARGS_MODE_DOTPLOT:
            return generate_dotplot;
    }
    return NULL;
}

#endif /* GENERATE_H_ */
