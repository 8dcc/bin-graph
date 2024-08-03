
#ifndef MAIN_H_
#define MAIN_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum EArgError {
    ARG_ERR_NONE  = 0,
    ARG_ERR_EXIT  = 1,
    ARG_ERR_USAGE = 2,
    ARG_ERR_HELP  = 3,
};

/* TODO: Add more modes: bigram_freq, etc. */
enum EProgramMode {
    MODE_GRAYSCALE,
    MODE_ASCII_LINEAR,
    MODE_ENTROPY,
    MODE_HISTOGRAM,
    MODE_BIGRAMS,
    MODE_DOTPLOT,
};

#define DEFAULT_SAMPLE_STEP  1
#define DEFAULT_BLOCK_SIZE   256
#define DEFAULT_OUTPUT_WIDTH 512
#define DEFAULT_OUTPUT_ZOOM  2

typedef struct ByteArray {
    uint8_t* data;
    size_t size;
} ByteArray;

/* Globals */
extern enum EProgramMode g_mode;
extern size_t g_offset_start;
extern size_t g_offset_end;
extern uint32_t g_sample_step;
extern uint32_t g_block_size;
extern uint32_t g_output_width;
extern uint32_t g_output_zoom;
extern uint32_t g_transform_squares_side;

#endif /* MAIN_H_ */
