#include "fir_filter.h"

#include <complex.h>
#include <math.h>
#include <string.h>

void init_filter_c(fir_filter_c_t *filter, const float *taps, size_t num_taps) {
  filter->taps = taps;
  filter->num_taps = num_taps;
  filter->delay_line = calloc(num_taps - 1, sizeof(float complex));
  filter->delay_line_num_samples = 0;
}

void apply_filter_c(fir_filter_c_t *filter, uint32_t decimation_factor,
                    const float complex *input, size_t input_len,
                    float complex **output, size_t *output_len) {
  const float *const taps = filter->taps;
  const size_t num_taps = filter->num_taps;
  float complex *delay_line = filter->delay_line;
  const size_t delay_line_num_samples = filter->delay_line_num_samples;

  const size_t num_output_samples = (size_t)(ceilf(
      (float)(input_len + delay_line_num_samples - num_taps + 1) /
      decimation_factor));

  float complex *output_buf =
      (float complex *)malloc(num_output_samples * sizeof(float complex));
  *output = output_buf;
  *output_len = num_output_samples;

  for (size_t output_index = 0; output_index < num_output_samples;
       ++output_index) {
    const int64_t input_start_index =
        -delay_line_num_samples + output_index * decimation_factor;
    const int64_t input_end_index = input_start_index + num_taps;
    size_t filter_index = 0;
    output_buf[output_index] = 0.0f + 0.0f * I;
    for (int64_t input_index = input_start_index;
         input_index != input_end_index; ++input_index) {
      if (input_index < 0) {
        const size_t delay_line_index = input_index + delay_line_num_samples;
        output_buf[output_index] +=
            taps[filter_index] * crealf(delay_line[delay_line_index]) +
            taps[filter_index] * cimagf(delay_line[delay_line_index]) * I;
      } else {
        output_buf[output_index] +=
            taps[filter_index] * crealf(input[input_index]) +
            taps[filter_index] * cimagf(input[input_index]) * I;
      }
      ++filter_index;
    }
  }

  memcpy(delay_line, &input[input_len - num_taps + 1],
         (num_taps - 1) * sizeof(float complex));
  filter->delay_line_num_samples = num_taps - 1;
}

void destroy_filter_c(fir_filter_c_t *filter) {
  free(filter->delay_line);
  filter->delay_line_num_samples = 0;
  filter->num_taps = 0;
  filter->taps = NULL;
}

void init_filter_r(fir_filter_r_t *filter, const float *taps, size_t num_taps) {
  filter->taps = taps;
  filter->num_taps = num_taps;
  filter->delay_line = calloc(num_taps - 1, sizeof(float));
  filter->delay_line_num_samples = 0;
}

void apply_filter_r(fir_filter_r_t *filter, uint32_t decimation_factor,
                    const float *input, size_t input_len, float **output,
                    size_t *output_len) {
  const float *const taps = filter->taps;
  const size_t num_taps = filter->num_taps;
  float *delay_line = filter->delay_line;
  const size_t delay_line_num_samples = filter->delay_line_num_samples;

  const size_t num_output_samples = (size_t)(ceilf(
      (float)(input_len + delay_line_num_samples - num_taps + 1) /
      decimation_factor));

  float *output_buf = (float *)malloc(num_output_samples * sizeof(float));
  *output = output_buf;
  *output_len = num_output_samples;

  for (size_t output_index = 0; output_index < num_output_samples;
       ++output_index) {
    const int64_t input_start_index =
        -delay_line_num_samples + output_index * decimation_factor;
    const int64_t input_end_index = input_start_index + num_taps;
    size_t filter_index = 0;
    output_buf[output_index] = 0.0f;
    for (int64_t input_index = input_start_index;
         input_index != input_end_index; ++input_index) {
      if (input_index < 0) {
        const size_t delay_line_index = input_index + delay_line_num_samples;
        output_buf[output_index] +=
            taps[filter_index] * delay_line[delay_line_index];
      } else {
        output_buf[output_index] += taps[filter_index] * input[input_index];
      }
      ++filter_index;
    }
  }

  memcpy(delay_line, &input[input_len - num_taps + 1],
         (num_taps - 1) * sizeof(float));
  filter->delay_line_num_samples = num_taps - 1;
}

void destroy_filter_r(fir_filter_r_t *filter) {
  free(filter->delay_line);
  filter->delay_line_num_samples = 0;
  filter->num_taps = 0;
  filter->taps = NULL;
}
