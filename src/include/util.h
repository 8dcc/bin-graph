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

#include <stdlib.h> /* exit() */
#include "liblog.h"

#define LENGTH(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

/* Print a fatal error and exit */
#define die(...)              \
    do {                      \
        log_ftl(__VA_ARGS__); \
        exit(1);              \
    } while (0)

/*----------------------------------------------------------------------------*/

/* Calculate the Shannon entropy of the specified bytes. Since log2() is used,
 * the return value is in the [0..8] range. */
double entropy(void* data, size_t data_sz);

#endif /* UTIL_H_ */
