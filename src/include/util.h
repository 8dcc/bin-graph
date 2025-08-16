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

#include <stdio.h>  /* printf, stderr */
#include <stdlib.h> /* exit */

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

#endif /* UTIL_H_ */
