#ifndef FM_H
#define FM_H

#include <complex.h>

#include "../concurrency/worker.h"
#include "../dsp/fir_filter.h"

typedef struct
{
    fir_filter_c_t input_filter;
    fir_filter_r_t audio_filter;
    float complex polar_discrim_prev_sample;
    float prev_deemph_input;
    float prev_deemph_output;
    worker_t worker;
} fm_demod_t;

void init_fm_demod(fm_demod_t* demod, interconnect_t* output);
void destroy_fm_demod(fm_demod_t* demod);

#endif //FM_H
