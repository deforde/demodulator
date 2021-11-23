#ifndef IQ_H
#define IQ_H

#include <complex.h>
#include <stddef.h>
#include <stdint.h>

typedef struct iq_data_t
{
    float complex* samples;
    size_t num_samples;
    uint32_t sample_rate_Hz;
} iq_data_t;

void destroy_iq_data(iq_data_t* iq_data);

#endif //IQ_H
