
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

#endif /* UTIL_H_ */
