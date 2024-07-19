
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
extern uint32_t g_output_width;
extern uint32_t g_output_zoom;
extern uint32_t g_sample_step;
extern bool g_average_sample;

#endif /* MAIN_H_ */
