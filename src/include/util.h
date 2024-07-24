
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

/* Return the size of a file using `fseek' */
size_t get_file_size(FILE* fp);

/* Calculate the Shannon entropy of the specified bytes. Since log2() is used,
 * the return value is in the [0..8] range. */
double entropy(void* data, size_t data_sz);

#endif /* UTIL_H_ */
