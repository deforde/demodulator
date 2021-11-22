#include "fft.h"
#include "plot.h"
#include "wav.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void do_plot(int16_t* samples, size_t num_samples)
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

int main()
{
    const char* filename = "/mnt/c/Users/dforde/dev/data/dsp/iq_recordings/fm/rds/SDRuno_20200907_184033Z_88110kHz.wav";

    int16_t* samples = NULL;
    size_t num_samples = 0;
    const bool result = ExtractSampleData(filename, &samples, &num_samples);
    if(!result) {
        return EXIT_FAILURE;
    }

    do_plot(samples, num_samples);

    return EXIT_SUCCESS;
}
