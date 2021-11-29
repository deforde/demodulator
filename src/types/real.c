#include "real.h"

#include <stdlib.h>

void destroy_real_data(real_data_t* real_data)
{
    free(real_data->samples);
    real_data->samples = NULL;
    real_data->sample_rate_Hz = 0;
    real_data->num_samples = 0;
}
