#include "fm.h"

#include <stdlib.h>

#define PI 3.14159265359F

void polar_discriminant(const float complex* const input, size_t input_len, float** output, size_t* output_len)
{
    *output_len = input_len;
    float* output_buffer = (float*)malloc(*output_len * sizeof(float));
    *output = output_buffer;

    float complex prev_sample = 0.0F + 0.0F*I;
    for(size_t i = 0; i < input_len; ++i) {
        output_buffer[i] = cargf(input[i] * conjf(prev_sample)) / PI;
        prev_sample = input[i];
    }
}
