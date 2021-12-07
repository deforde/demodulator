#include "diff_man.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void init_diff_man_decoder(diff_man_decoder_t* decoder)
{
    decoder->prev_bit = 0;
    decoder->clock_phase = 0;
    decoder->num_history_bits = 0;
    decoder->bit_index = 0;
    memset(decoder->bit_history, 0, sizeof(decoder->bit_history));
}

bool decode_diff_man(diff_man_decoder_t* decoder, uint32_t bit, uint32_t* data_bit_out)
{
    const bool transition = bit != decoder->prev_bit;
    bool data_bit_produced = false;

    if(decoder->bit_index % 2 != decoder->clock_phase) {
        data_bit_produced = true;
        *data_bit_out = transition ? 0 : 1;
    }

    ++decoder->bit_index;
    decoder->prev_bit = bit;

    decoder->bit_history[decoder->num_history_bits++] = bit;
    if(decoder->num_history_bits == sizeof(decoder->bit_history) / sizeof(*decoder->bit_history)) {
        size_t even_count = 0;
        size_t odd_count = 0;

        for(size_t i = 0; i < decoder->num_history_bits; ++i) {
            if(decoder->bit_history[i] == 1) {
                if(i % 2 == 0) {
                    ++even_count;
                }
                else {
                    ++odd_count;
                }
            }
        }

        if(even_count > odd_count) {
            decoder->clock_phase = 0;
        }
        else if(odd_count > even_count) {
            decoder->clock_phase = 1;
        }

        decoder->num_history_bits = 0;
        memset(decoder->bit_history, 0, sizeof(decoder->bit_history));
    }

    return data_bit_produced;
}

void destroy_diff_man_decoder(diff_man_decoder_t* decoder)
{
    decoder->prev_bit = 0;
    decoder->clock_phase = 0;
    decoder->num_history_bits = 0;
    decoder->bit_index = 0;
    memset(decoder->bit_history, 0, sizeof(decoder->bit_history));
}
