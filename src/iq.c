#include "iq.h"

#include <stdlib.h>

void destroy_iq_data(iq_data_t* iq_data)
{
    free(iq_data->samples);
    iq_data->samples = NULL;
    iq_data->sample_rate_Hz = 0;
    iq_data->num_samples = 0;
}
