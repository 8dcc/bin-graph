
#ifndef READ_FILE_H_
#define READ_FILE_H_ 1

#include <stddef.h>
#include <stdint.h>

typedef struct ByteArray {
    uint8_t* data;
    size_t size;
} ByteArray;

/*----------------------------------------------------------------------------*/

/*
 * Return the size of a file using `fseek'.
 */
long get_file_size(FILE* fp);

/*
 * Validate and translate the offsets of a file.
 * It validates that:
 *   - Both offsets are within the file limits.
 *   - The end offset is greater or equal than the start offset.
 * It also translates:
 *   - A zero end offset, which translates to the EOF.
 */
void get_real_offsets(FILE* fp, long* offset_start, long* offset_end);

/*
 * Read the bytes of a file in a linear way from the starting offset to the end
 * offset. Internally calls `get_real_offsets' to ensure they are valid.
 */
void read_file(ByteArray* dst, FILE* fp, long offset_start, long offset_end);

/*
 * Initialize a ByteArray structure for reading a chunk of a file. Internally
 * calls `get_real_offsets' to ensure they are valid. It must be freed with
 * `byte_array_free'.
 */
void byte_array_init(ByteArray* bytes, FILE* fp, long offset_start,
                     long offset_end);

/*
 * Free all members of a ByteArray structure. Doesn't free the ByteArray itself.
 */
void byte_array_free(ByteArray* bytes);

#endif /* READ_FILE_H_ */
