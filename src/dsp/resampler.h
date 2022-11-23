#ifndef RESAMPLER_H
#define RESAMPLER_H

#include <complex.h>
#include <stddef.h>
#include <stdint.h>

#include "../types/iq.h"
#include "../types/real.h"
#include "fir_filter.h"

typedef struct {
  fir_filter_r_t filter;
  uint32_t interpolation_factor;
  uint32_t decimation_factor;
  float *historic_samples;
  size_t num_historic_samples;
} resampler_r_t;

typedef struct {
  fir_filter_c_t filter;
  uint32_t interpolation_factor;
  uint32_t decimation_factor;
  float complex *historic_samples;
  size_t num_historic_samples;
} resampler_c_t;

void init_resampler_r(resampler_r_t *resampler, uint32_t interpolation_factor,
                      uint32_t decimation_factor, const float *taps,
                      size_t num_taps);
void apply_resampler_r(resampler_r_t *resampler, const real_data_t *input,
                       real_data_t *output);
void destroy_resampler_r(resampler_r_t *resampler);

void init_resampler_c(resampler_c_t *resampler, uint32_t interpolation_factor,
                      uint32_t decimation_factor, const float *taps,
                      size_t num_taps);
void apply_resampler_c(resampler_c_t *resampler, const iq_data_t *input,
                       iq_data_t *output);
void destroy_resampler_c(resampler_c_t *resampler);

#endif // RESAMPLER_H
