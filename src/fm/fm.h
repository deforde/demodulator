#ifndef FM_H
#define FM_H

#include <complex.h>

#include "../concurrency/interconnect.h"
#include "../concurrency/worker.h"
#include "../dsp/fir_filter.h"
#include "../dsp/resampler.h"
#include "bpsk.h"

typedef struct {
  fir_filter_c_t input_filter;
  fir_filter_r_t stereo_delta_channel_filter;

  resampler_r_t mono_audio_resampler;
  resampler_r_t stereo_audio_resampler;

  float complex polar_discrim_prev_sample;

  float prev_mono_deemph_input;
  float prev_mono_deemph_output;
  float prev_stereo_deemph_input;
  float prev_stereo_deemph_output;

  bpsk_demod_t bpsk_demod;

  worker_t worker;
} fm_demod_t;

void init_fm_demod(fm_demod_t *demod, interconnect_t *output);
void destroy_fm_demod(fm_demod_t *demod);

#endif // FM_H
