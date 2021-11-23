#ifndef FFT_H
#define FFT_H

#include <fftw3.h>

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct fft_desc_t
{
    size_t len;
    fftw_complex* output;
    fftw_plan plan;
    fftw_complex* scratch;
} fft_desc_t;

void init_fft(fft_desc_t* fft, size_t len);

bool execute_fft(fft_desc_t* fft, const float complex* iq_buf, size_t num_samples);

void destroy_fft(fft_desc_t* fft);

#endif //FFT_H
