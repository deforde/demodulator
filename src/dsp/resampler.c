#include "resampler.h"

#include <stdlib.h>
#include <string.h>

#include "../types/iq.h"
#include "../types/real.h"

void init_resampler_r(resampler_r_t* resampler, uint32_t interpolation_factor, uint32_t decimation_factor, const float* const taps, size_t num_taps)
{
    resampler->interpolation_factor = interpolation_factor;
    resampler->decimation_factor = decimation_factor;
    resampler->historic_samples = (float*)malloc((decimation_factor - 1) * sizeof(float));
    resampler->num_historic_samples = 0;
    init_filter_r(&resampler->filter, taps, num_taps);
}

void apply_resampler_r(resampler_r_t* resampler, const real_data_t* const input, real_data_t* const output)
{
    const uint32_t interpolation_factor = resampler->interpolation_factor;

    real_data_t upsampled_data;
    upsampled_data.sample_rate_Hz = input->sample_rate_Hz * interpolation_factor;
    upsampled_data.num_samples = input->num_samples * interpolation_factor;
    if(interpolation_factor > 1) {
        upsampled_data.samples = (float*)malloc(upsampled_data.num_samples * sizeof(float));
        memset(upsampled_data.samples, 0, upsampled_data.num_samples * sizeof(float));
        for(size_t i = 0; i < upsampled_data.num_samples; i += interpolation_factor) {
            upsampled_data.samples[i] = input->samples[i / interpolation_factor];
        }
    }
    else {
        upsampled_data.samples = input->samples;
    }

    const uint32_t decimation_factor = resampler->decimation_factor;
    const size_t num_hist_samples = resampler->num_historic_samples;
    float* hist = resampler->historic_samples;

    const size_t total_num_samples = (num_hist_samples + upsampled_data.num_samples) / decimation_factor * decimation_factor;
    const size_t samples_to_carry_over = (num_hist_samples + upsampled_data.num_samples) % decimation_factor;
    float* accumulated_data = (float*)malloc(total_num_samples * sizeof(float));
    memcpy(accumulated_data, hist, num_hist_samples * sizeof(float));
    memcpy(accumulated_data + num_hist_samples, upsampled_data.samples, (total_num_samples - num_hist_samples) * sizeof(float));
    memcpy(hist, &upsampled_data.samples[upsampled_data.num_samples - samples_to_carry_over], samples_to_carry_over * sizeof(float));
    resampler->num_historic_samples = samples_to_carry_over;

    destroy_real_data(&upsampled_data);

    output->sample_rate_Hz = input->sample_rate_Hz * interpolation_factor / decimation_factor;
    fir_filter_r_t filter = resampler->filter;
    apply_filter_r(&filter, decimation_factor, accumulated_data, total_num_samples, &output->samples, &output->num_samples);

    free(accumulated_data);
}

void destroy_resampler_r(resampler_r_t* resampler)
{
    destroy_filter_r(&resampler->filter);
    resampler->interpolation_factor = 1;
    resampler->decimation_factor = 1;
    free(resampler->historic_samples);
    resampler->num_historic_samples = 0;
}

void init_resampler_c(resampler_c_t* resampler, uint32_t interpolation_factor, uint32_t decimation_factor, const float* const taps, size_t num_taps)
{
    resampler->interpolation_factor = interpolation_factor;
    resampler->decimation_factor = decimation_factor;
    resampler->historic_samples = (float complex*)malloc((decimation_factor - 1) * sizeof(float complex));
    resampler->num_historic_samples = 0;
    init_filter_c(&resampler->filter, taps, num_taps);
}

void apply_resampler_c(resampler_c_t* resampler, const iq_data_t* const input, iq_data_t* const output)
{
    const uint32_t interpolation_factor = resampler->interpolation_factor;

    iq_data_t upsampled_data;
    upsampled_data.sample_rate_Hz = input->sample_rate_Hz * interpolation_factor;
    upsampled_data.num_samples = input->num_samples * interpolation_factor;
    if(interpolation_factor > 1) {
        upsampled_data.samples = (float complex*)malloc(upsampled_data.num_samples * sizeof(float complex));
        memset(upsampled_data.samples, 0, upsampled_data.num_samples * sizeof(float complex));
        for(size_t i = 0; i < upsampled_data.num_samples; i += interpolation_factor) {
            upsampled_data.samples[i] = input->samples[i / interpolation_factor];
        }
    }
    else {
        upsampled_data.samples = input->samples;
    }

    const uint32_t decimation_factor = resampler->decimation_factor;
    const size_t num_hist_samples = resampler->num_historic_samples;
    float complex* hist = resampler->historic_samples;

    const size_t total_num_samples = (num_hist_samples + upsampled_data.num_samples) / decimation_factor * decimation_factor;
    const size_t samples_to_carry_over = (num_hist_samples + upsampled_data.num_samples) % decimation_factor;
    float complex* accumulated_data = (float complex*)malloc(total_num_samples * sizeof(float complex));
    memcpy(accumulated_data, hist, num_hist_samples * sizeof(float complex));
    memcpy(accumulated_data + num_hist_samples, upsampled_data.samples, (total_num_samples - num_hist_samples) * sizeof(float complex));
    memcpy(hist, &upsampled_data.samples[upsampled_data.num_samples - samples_to_carry_over], samples_to_carry_over * sizeof(float complex));
    resampler->num_historic_samples = samples_to_carry_over;

    destroy_iq_data(&upsampled_data);

    output->sample_rate_Hz = input->sample_rate_Hz * interpolation_factor / decimation_factor;
    fir_filter_c_t filter = resampler->filter;
    apply_filter_c(&filter, decimation_factor, accumulated_data, total_num_samples, &output->samples, &output->num_samples);

    free(accumulated_data);
}

void destroy_resampler_c(resampler_c_t* resampler)
{
    destroy_filter_c(&resampler->filter);
    resampler->interpolation_factor = 1;
    resampler->decimation_factor = 1;
    free(resampler->historic_samples);
    resampler->num_historic_samples = 0;
}
