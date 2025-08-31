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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> /* log2() */

#include "include/util.h"

double entropy(void* data, size_t data_sz) {
    size_t* occurrences = calloc(256, sizeof(size_t));
    if (occurrences == NULL)
        return 0.0;

    /* Count the occurrences of each byte in the input */
    for (size_t i = 0; i < data_sz; i++) {
        const uint8_t byte = ((uint8_t*)data)[i];
        occurrences[byte]++;
    }

    double result = 0.0;
    for (int byte = 0; byte < 256; byte++) {
        if (occurrences[byte] == 0)
            continue;

        /* Probablity of encountering this byte on the input */
        const double probability = (double)occurrences[byte] / data_sz;

        /* Accumulate entropy of each possible value (00..FF) */
        result -= probability * log2(probability);
    }

    free(occurrences);
    return result;
}
