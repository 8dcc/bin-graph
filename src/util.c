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
#include <limits.h>
#include <math.h> /* log2() */

#include "include/util.h"
#include "include/image.h" /* Color */

double entropy(void* data, size_t data_sz) {
    size_t* occurrences = calloc(UCHAR_MAX + 1, sizeof(size_t));
    if (occurrences == NULL)
        return 0.0;

    /* Count the occurrences of each byte in the input */
    for (size_t i = 0; i < data_sz; i++) {
        const uint8_t byte = ((uint8_t*)data)[i];
        occurrences[byte]++;
    }

    double result = 0.0;
    for (int byte = 0; byte < UCHAR_MAX + 1; byte++) {
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

void* reverse_buffer(void* buf, size_t num_elems, size_t elem_sz) {
    uint8_t* casted             = (uint8_t*)buf;
    const size_t half_num_elems = num_elems / 2;

    /* Temporary buffer for swapping values */
    uint8_t* tmp = malloc(elem_sz);
    if (tmp == NULL)
        return NULL;

    /*
     * Go from 'buf[0]' to 'buf[size/2]' and replace each byte with the byte
     * from the opposite end:
     *
     *     s = [ H E L L O ] => [ O L L E H ]
     *           | |   | |
     *           | ----- |
     *           |-------|
     */
    for (size_t i = 0; i < half_num_elems; i++) {
        const size_t raw_start_idx = i * elem_sz;
        const size_t raw_end_idx   = (num_elems - 1 - i) * elem_sz;

        memcpy(tmp, &casted[raw_start_idx], elem_sz);
        memcpy(&casted[raw_start_idx], &casted[raw_end_idx], elem_sz);
        memcpy(&casted[raw_end_idx], tmp, elem_sz);
    }

    free(tmp);
    return buf;
}

Color from_intensity(uint8_t value) {
#ifdef BIN_GRAPH_HEATMAP
    /*
     * The heatmap visualization uses a linear scale for blue values, but an
     * exponential scale for red values. This representation is more closely
     * related to how entropy is calculated (using a base 2 logarithm); in other
     * words, brighter values are exponentially more significant/informative
     * than darker values.
     */
    return (Color){
        .r = (uint8_t)(pow((double)value / UCHAR_MAX, 3) * 255.0),
        .g = 0,
        .b = value,
    };
#else  /* not BIN_GRAPH_HEATMAP */
    return (Color){
        .r = value,
        .g = value,
        .b = value,
    };
#endif /* not BIN_GRAPH_HEATMAP */
}
