
#ifndef MAIN_H_
#define MAIN_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct ByteArray {
    uint8_t* data;
    size_t size;
} ByteArray;

/* Globals */
extern size_t g_offset_start;
extern size_t g_offset_end;
extern uint32_t g_sample_step;
extern uint32_t g_block_size;
extern uint32_t g_output_width;
extern uint32_t g_output_zoom;

#endif /* MAIN_H_ */
