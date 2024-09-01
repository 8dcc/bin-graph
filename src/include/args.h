
#ifndef ARGS_H_
#define ARGS_H_ 1

#include <stddef.h>
#include <stdint.h>

enum EArgError {
    ARG_ERR_NONE  = 0,
    ARG_ERR_EXIT  = 1,
    ARG_ERR_USAGE = 2,
    ARG_ERR_HELP  = 3,
};

/* TODO: Add more modes: bigram_freq, etc. */
enum EProgramMode {
    MODE_GRAYSCALE,
    MODE_ASCII,
    MODE_ENTROPY,
    MODE_HISTOGRAM,
    MODE_BIGRAMS,
    MODE_DOTPLOT,
};

#define DEFAULT_BLOCK_SIZE   256
#define DEFAULT_OUTPUT_WIDTH 512
#define DEFAULT_OUTPUT_ZOOM  2

/* Globals */
extern enum EProgramMode g_mode;
extern size_t g_offset_start;
extern size_t g_offset_end;
extern uint32_t g_block_size;
extern uint32_t g_output_width;
extern uint32_t g_output_zoom;
extern uint32_t g_transform_squares_side;

/*----------------------------------------------------------------------------*/

void parse_args(int argc, char** argv);

#endif /* ARGS_H_ */
