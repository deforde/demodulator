#ifndef BPSK_H
#define BPSK_H

#include <liquid/liquid.h>
#include <stdint.h>

#include "../types/real.h"

typedef struct {
    resamp_rrrf resampler;
    firfilt_crcf filter;
    agc_crcf agc;
    nco_crcf oscillator;
    symsync_crcf sym_sync;
    modemcf modem;
    uint64_t sample_count;
    float resampling_ratio;
} bpsk_demod_t;

void init_bpsk_demod(bpsk_demod_t* demod);
void demodulate_bpsk(bpsk_demod_t* demod, const real_data_t* input_data);
void destroy_bpsk_demod(bpsk_demod_t* demod);

#endif //BPSK_H
