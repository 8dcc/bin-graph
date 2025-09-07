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

#ifndef TRANSFORM_H_
#define TRANSFORM_H_ 1

#include "image.h"
#include "args.h"

/*
 * Pointer to a function that transforms an 'Image' depending on the program
 * arguments.
 *
 * TODO: Return boolean.
 */
typedef void (*transformation_func_ptr_t)(const Args* args, Image* image);

/*
 * Group the data of a linear image into squares of side N. If the image
 * dimensions are not divisible by N, they will be increased.
 */
void transform_squares(const Args* args, Image* image);

/*
 * Transform the image in a linear image into a ZigZag pattern, reversing odd
 * rows.
 */
void transform_zigzag(const Args* args, Image* image);

/*
 * Transform the specified input string into an output grid buffer using a
 * space-filling Hilbert curve, with the specified recursion level.
 */
void transform_hilbert(const Args* args, Image* image);

/*----------------------------------------------------------------------------*/

/*
 * Return a pointer to the transformation function depending on the specified
 * program arguments.
 */
static inline transformation_func_ptr_t transformation_func_from_args(
  const Args* args) {
    if (args->transform_squares_side > 1)
        return transform_squares;
    if (args->transform_zigzag)
        return transform_zigzag;
    if (args->transform_hilbert_level >= 1)
        return transform_hilbert;
    return NULL;
}

#endif /* TRANSFORM_H_ */
