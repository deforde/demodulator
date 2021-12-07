#ifndef DIFF_MAN_H
#define DIFF_MAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Differential manchester decoder

typedef struct {
    uint32_t prev_bit;
    uint32_t clock_phase;
    size_t bit_index;
    size_t num_transition_history_samples;
    uint32_t transition_history[128];
} diff_man_decoder_t;

void init_diff_man_decoder(diff_man_decoder_t* decoder);
bool decode_diff_man(diff_man_decoder_t* decoder, uint32_t bit, uint32_t* data_bit_out);
void destroy_diff_man_decoder(diff_man_decoder_t* decoder);

#endif //DIFF_MAN_H
