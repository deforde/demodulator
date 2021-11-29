#ifndef FFT_H
#define FFT_H

#include <fftw3.h>  //Needs to be included before complex.h to ensure that fftw_complex is defined

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    size_t len;
    fftw_complex* output;
    fftw_plan plan;
    fftw_complex* scratch;
} fft_desc_c_t;

typedef struct
{
    size_t len;
    double* input;
    fftw_complex* output;
    fftw_plan plan;
} fft_desc_r_t;

void init_fft_c(fft_desc_c_t* fft, size_t len);
bool execute_fft_c(fft_desc_c_t* fft, const float complex* iq_buf, size_t num_samples);
void destroy_fft_c(fft_desc_c_t* fft);

void init_fft_r(fft_desc_r_t* fft, size_t len);
bool execute_fft_r(fft_desc_r_t* fft, const float* samples, size_t num_samples);
void destroy_fft_r(fft_desc_r_t* fft);

#endif //FFT_H
