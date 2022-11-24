#include "resampler.h"

#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../types/iq.h"
#include "../types/real.h"

void init_resampler_r(resampler_r_t *resampler, uint32_t interpolation_factor,
                      uint32_t decimation_factor, const float *taps,
                      size_t num_taps) {
  resampler->interpolation_factor = interpolation_factor;
  resampler->decimation_factor = decimation_factor;
  if (decimation_factor > 1) {
    resampler->historic_samples = calloc(decimation_factor - 1, sizeof(float));
  } else {
    resampler->historic_samples = NULL;
  }
  resampler->num_historic_samples = 0;
  init_filter_r(&resampler->filter, taps, num_taps);
}

void apply_resampler_r(resampler_r_t *resampler, const real_data_t *input,
                       real_data_t *output) {
  const uint32_t interpolation_factor = resampler->interpolation_factor;
  const bool upsampling_reqiured = interpolation_factor > 1;

  float *stage_1_data = NULL;
  size_t stage_1_num_samples = 0;

  if (upsampling_reqiured) {
    stage_1_num_samples = input->num_samples * interpolation_factor;
    stage_1_data = calloc(stage_1_num_samples, sizeof(float));
    for (size_t i = 0; i < stage_1_num_samples; i += interpolation_factor) {
      stage_1_data[i] = input->samples[i / interpolation_factor];
    }
  } else {
    stage_1_data = input->samples;
    stage_1_num_samples = input->num_samples;
  }

  const uint32_t decimation_factor = resampler->decimation_factor;
  const bool downsampling_required = decimation_factor > 1;

  float *stage_2_data = NULL;
  size_t stage_2_num_samples = 0;

  if (downsampling_required) {
    const size_t num_hist_samples = resampler->num_historic_samples;
    float *hist = resampler->historic_samples;

    stage_2_num_samples = (num_hist_samples + stage_1_num_samples) /
                          decimation_factor * decimation_factor;
    const size_t samples_to_carry_over =
        (num_hist_samples + stage_1_num_samples) % decimation_factor;
    stage_2_data = (float *)malloc(stage_2_num_samples * sizeof(float));
    memcpy(stage_2_data, hist, num_hist_samples * sizeof(float));
    memcpy(stage_2_data + num_hist_samples, stage_1_data,
           (stage_2_num_samples - num_hist_samples) * sizeof(float));
    memcpy(hist, &stage_1_data[stage_1_num_samples - samples_to_carry_over],
           samples_to_carry_over * sizeof(float));
    resampler->num_historic_samples = samples_to_carry_over;
  } else {
    stage_2_data = stage_1_data;
    stage_2_num_samples = stage_1_num_samples;
  }

  output->sample_rate_Hz =
      input->sample_rate_Hz * interpolation_factor / decimation_factor;
  fir_filter_r_t filter = resampler->filter;
  apply_filter_r(&filter, decimation_factor, stage_2_data, stage_2_num_samples,
                 &output->samples, &output->num_samples);

  if (downsampling_required) {
    free(stage_2_data);
  }
  if (upsampling_reqiured) {
    free(stage_1_data);
  }
}

void destroy_resampler_r(resampler_r_t *resampler) {
  destroy_filter_r(&resampler->filter);
  resampler->interpolation_factor = 1;
  resampler->decimation_factor = 1;
  if (resampler->historic_samples != NULL) {
    free(resampler->historic_samples);
  }
  resampler->num_historic_samples = 0;
}

void init_resampler_c(resampler_c_t *resampler, uint32_t interpolation_factor,
                      uint32_t decimation_factor, const float *taps,
                      size_t num_taps) {
  resampler->interpolation_factor = interpolation_factor;
  resampler->decimation_factor = decimation_factor;
  resampler->historic_samples =
      (float complex *)malloc((decimation_factor - 1) * sizeof(float complex));
  resampler->num_historic_samples = 0;
  init_filter_c(&resampler->filter, taps, num_taps);
}

void apply_resampler_c(resampler_c_t *resampler, const iq_data_t *input,
                       iq_data_t *output) {
  const uint32_t interpolation_factor = resampler->interpolation_factor;
  const bool upsampling_reqiured = interpolation_factor > 1;

  float complex *stage_1_data = NULL;
  size_t stage_1_num_samples = 0;

  if (upsampling_reqiured) {
    stage_1_num_samples = input->num_samples * interpolation_factor;
    stage_1_data = calloc(stage_1_num_samples, sizeof(float complex));
    for (size_t i = 0; i < stage_1_num_samples; i += interpolation_factor) {
      stage_1_data[i] = input->samples[i / interpolation_factor];
    }
  } else {
    stage_1_data = input->samples;
    stage_1_num_samples = input->num_samples;
  }

  const uint32_t decimation_factor = resampler->decimation_factor;
  const bool downsampling_required = decimation_factor > 1;

  float complex *stage_2_data = NULL;
  size_t stage_2_num_samples = 0;

  if (downsampling_required) {
    const size_t num_hist_samples = resampler->num_historic_samples;
    float complex *hist = resampler->historic_samples;

    stage_2_num_samples = (num_hist_samples + stage_1_num_samples) /
                          decimation_factor * decimation_factor;
    const size_t samples_to_carry_over =
        (num_hist_samples + stage_1_num_samples) % decimation_factor;
    stage_2_data =
        (float complex *)malloc(stage_2_num_samples * sizeof(float complex));
    memcpy(stage_2_data, hist, num_hist_samples * sizeof(float complex));
    memcpy(stage_2_data + num_hist_samples, stage_1_data,
           (stage_2_num_samples - num_hist_samples) * sizeof(float complex));
    memcpy(hist, &stage_1_data[stage_1_num_samples - samples_to_carry_over],
           samples_to_carry_over * sizeof(float complex));
    resampler->num_historic_samples = samples_to_carry_over;
  } else {
    stage_2_data = stage_1_data;
    stage_2_num_samples = stage_1_num_samples;
  }

  output->sample_rate_Hz =
      input->sample_rate_Hz * interpolation_factor / decimation_factor;
  fir_filter_c_t filter = resampler->filter;
  apply_filter_c(&filter, decimation_factor, stage_2_data, stage_2_num_samples,
                 &output->samples, &output->num_samples);

  if (downsampling_required) {
    free(stage_2_data);
  }
  if (upsampling_reqiured) {
    free(stage_1_data);
  }
}

void destroy_resampler_c(resampler_c_t *resampler) {
  destroy_filter_c(&resampler->filter);
  resampler->interpolation_factor = 1;
  resampler->decimation_factor = 1;
  free(resampler->historic_samples);
  resampler->num_historic_samples = 0;
}
