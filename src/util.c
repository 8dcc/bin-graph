
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* log2() */

#include "include/util.h"

double entropy(void* data, size_t data_sz) {
    uint32_t* occurrences = calloc(256, sizeof(uint32_t));

    /* Count the occurrences of each byte in the input */
    for (size_t i = 0; i < data_sz; i++) {
        const uint8_t byte = ((uint8_t*)data)[i];
        occurrences[byte]++;
    }

    double result = 0.0;

    /*
     * NOTE: For more information, see my article about entropy:
     *   https://8dcc.github.io/programming/understanding-entropy.html
     */
    for (int byte = 0; byte < 256; byte++) {
        if (occurrences[byte] == 0)
            continue;

        /* Probablity of encountering this byte on the input */
        const double probability = (double)occurrences[byte] / data_sz;

        /* Accumulate entropy of each possible value (00..FF) */
        result -= probability * log2(probability);
    }

    /* We are done with the occurrences array */
    free(occurrences);

    return result;
}
