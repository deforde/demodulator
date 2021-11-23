#ifndef AUDIO_H
#define AUDIO_H

#include <stddef.h>
#include <stdint.h>

typedef struct audio_data_t
{
    float* samples;
    size_t num_samples;
    uint32_t sample_rate_Hz;
} audio_data_t;

void destroy_audio_data(audio_data_t* audio_data);

#endif //AUDIO_H
