#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <complex.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct fir_filter_r_t
{
    const float* taps;
    size_t num_taps;
    float* delay_line;
    size_t delay_line_num_samples;
} fir_filter_r_t;

typedef struct fir_filter_c_t
{
    const float* taps;
    size_t num_taps;
    float complex* delay_line;
    size_t delay_line_num_samples;
} fir_filter_c_t;

void init_filter_c(fir_filter_c_t* filter, const float* taps, size_t num_taps);

void apply_filter_c(fir_filter_c_t* filter, uint32_t decimation_factor, const float complex* input, size_t input_len, float complex** output, size_t* output_len);

void destroy_filter_c(fir_filter_c_t* filter);

void init_filter_r(fir_filter_r_t* filter, const float* taps, size_t num_taps);

void apply_filter_r(fir_filter_r_t* filter, uint32_t decimation_factor, const float* input, size_t input_len, float** output, size_t* output_len);

void destroy_filter_r(fir_filter_r_t* filter);

#endif //FIR_FILTER_H
