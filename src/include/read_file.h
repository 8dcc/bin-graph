
#ifndef READ_FILE_H_
#define READ_FILE_H_ 1

#include <stddef.h>
#include <stdint.h>

typedef struct ByteArray {
    uint8_t* data;
    size_t size;
} ByteArray;

/*----------------------------------------------------------------------------*/

/* Return the size of a file using `fseek' */
long get_file_size(FILE* fp);

/* Read the bytes of a file in a linear way from the starting offset to the end
 * offset. Zero can be used as the end offset to read until EOF. */
ByteArray read_file(FILE* fp, long offset_start, long offset_end);

#endif /* READ_FILE_H_ */
