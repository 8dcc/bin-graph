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

#ifndef UTIL_H_
#define UTIL_H_ 1

#include <stdint.h>
#include <stdio.h>  /* printf, stderr */
#include <stdlib.h> /* exit */

#include "image.h" /* Color */

/*
 * Maximum value returned by a function that calculates the entropy of an array
 * of bytes using a base 2 logarithm. For more information, see:
 * https://8dcc.github.io/programming/understanding-entropy.html#entropy-range
 */
#define MAX_ENTROPY 8.0

/*----------------------------------------------------------------------------*/

/*
 * Mark a symbol as unused in the current scope.
 */
#define UNUSED(SYM) ((void)SYM)

/*
 * Obtain the compile-time length of an array.
 */
#define LENGTH(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

/*
 * Print a warning with the specified format, along with the program name and a
 * newline.
 */
#define WRN(...)                                                               \
    do {                                                                       \
        fprintf(stderr, "bin-graph: Warning: ");                               \
        fprintf(stderr, __VA_ARGS__);                                          \
        fputc('\n', stderr);                                                   \
    } while (0)

/*
 * Print an error with the specified format, along with the program name and a
 * newline.
 */
#define ERR(...)                                                               \
    do {                                                                       \
        fprintf(stderr, "bin-graph: Error: ");                                 \
        fprintf(stderr, __VA_ARGS__);                                          \
        fputc('\n', stderr);                                                   \
    } while (0)

/*
 * Print an error and exit unsuccessfully.
 */
#define DIE(...)                                                               \
    do {                                                                       \
        ERR(__VA_ARGS__);                                                      \
        exit(1);                                                               \
    } while (0)

/*----------------------------------------------------------------------------*/

/*
 * Calculate the Shannon entropy of the specified bytes. Since log2() is used,
 * the return value is in the [0..8] range.
 *
 * For more information, see my article about entropy:
 * https://8dcc.github.io/programming/understanding-entropy.html
 */
double entropy(void* data, size_t data_sz);

/*
 * Reverse a buffer in-place, with the specified number of elements of the
 * specified size each. Returns the 'buf' argument on success, or NULL on error.
 */
void* reverse_buffer(void* buf, size_t num_elems, size_t elem_sz);

/*
 * Get a color from the intensity of a byte.
 */
Color from_intensity(uint8_t value);

#endif /* UTIL_H_ */
