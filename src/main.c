#include "fft.h"
#include "fir_filter.h"
#include "plot.h"
#include "wav.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PI 3.14159265359F

void do_plot(float* samples, size_t num_samples)
{
    const size_t fft_len = 4096;
    size_t num_ffts = num_samples / 2 / fft_len;
    fft_desc_t fft;
    init_fft(&fft, fft_len);
    float* amplitude_spectrum = (float*)malloc(sizeof(float) * fft.len);
    for(size_t i = 0; i < num_ffts; ++i) {
        const bool result = execute_fft(&fft, samples + i * fft_len * 2, fft_len * 2);
        if(!result) {
            break;
        }

        for(size_t i = 0; i < fft.len; ++i) {
            amplitude_spectrum[i] = (float)(10 * log10(sqrt(fft.output[i][0] * fft.output[i][0] + fft.output[i][1] * fft.output[i][1]) + DBL_MIN));
        }

        system("clear");
        plot_amplitude_spectrum(amplitude_spectrum, fft.len);

        const struct timespec tim = { .tv_sec = 0, .tv_nsec = 100000000 };
        nanosleep(&tim, NULL);
    }
    free(amplitude_spectrum);
    destroy_fft(&fft);
}

typedef struct audio_data_t
{
    float* samples;
    size_t num_samples;
    uint32_t sample_rate_Hz;
} audio_data_t;

void polar_discriminant(const float* const input, size_t input_len, float** output, size_t* output_len)
{
    float p_r = 0.0F;
    float p_i = 0.0F;

    *output_len = input_len / 2;
    float* output_buffer = (float*)malloc(*output_len * sizeof(float));
    *output = output_buffer;

    for(size_t i = 0; i < input_len; i += 2) {
        const float c_r = input[i];
        const float c_i = input[i+1];

        const float conj_prod_r = c_r * p_r + c_i * p_i;
        const float conj_prod_i = c_i * p_r - c_r * p_i;

        const float ans = atanf(conj_prod_i / conj_prod_r) / PI;
        p_r = c_r;
        p_i = c_i;

        output_buffer[i / 2] = ans;
    }
}

int main()
{
    const char* filename = "/mnt/c/Users/dforde/dev/data/dsp/iq_recordings/fm/rds/SDRuno_20200907_184033Z_88110kHz.wav";

    iq_data_t iq_data;
    const bool result = ExtractSampleData(filename, &iq_data);
    if(!result) {
        return EXIT_FAILURE;
    }

    fir_filter_t mono_audio_filter;
    init_filter(&mono_audio_filter, FIR_FILT_250kFS_15kPA_19kST, N_FIR_FILT_250kFS_15kPA_19kST);

    uint32_t decimation_factor = 1;
    iq_data_t filtered_iq_data;
    filtered_iq_data.sample_rate_Hz = iq_data.sample_rate_Hz / decimation_factor;

    apply_filter(&mono_audio_filter, decimation_factor, iq_data.samples, iq_data.num_samples, &filtered_iq_data.samples, &filtered_iq_data.num_samples);
    destroy_filter(&mono_audio_filter);

    audio_data_t audio_data;
    audio_data.sample_rate_Hz = filtered_iq_data.sample_rate_Hz;

    polar_discriminant(filtered_iq_data.samples, filtered_iq_data.num_samples, &audio_data.samples, &audio_data.num_samples);

    fir_filter_t audio_filter;
    init_filter(&audio_filter, FIR_FILT_250kFS_25kPA_32kST, N_FIR_FILT_250kFS_25kPA_32kST);

    decimation_factor = 4;
    audio_data_t filtered_audio_data;
    filtered_audio_data.sample_rate_Hz = audio_data.sample_rate_Hz / decimation_factor;

    apply_filter(&audio_filter, decimation_factor, audio_data.samples, audio_data.num_samples, &filtered_audio_data.samples, &filtered_audio_data.num_samples);
    destroy_filter(&audio_filter);

    // do_plot(filtered_iq_data.samples, filtered_iq_data.num_samples);

    FILE* fp = fopen("audio.bin", "wb");
    if(!fp) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
    } else {
        fwrite(filtered_audio_data.samples, 1, filtered_audio_data.num_samples * sizeof(float), fp);
        fclose(fp);
    }

    free(audio_data.samples);
    audio_data.samples = NULL;
    audio_data.sample_rate_Hz = 0;
    audio_data.num_samples = 0;

    free(filtered_iq_data.samples);
    filtered_iq_data.samples = NULL;
    filtered_iq_data.sample_rate_Hz = 0;
    filtered_iq_data.num_samples = 0;

    free(iq_data.samples);
    iq_data.samples = NULL;
    iq_data.sample_rate_Hz = 0;
    iq_data.num_samples = 0;

    return EXIT_SUCCESS;
}
