#include "audio.h"

#include <stdlib.h>

void destroy_audio_data(audio_data_t* audio_data)
{
    free(audio_data->samples);
    audio_data->samples = NULL;
    audio_data->sample_rate_Hz = 0;
    audio_data->num_samples = 0;
}
