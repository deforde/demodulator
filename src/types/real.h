#ifndef REAL_H
#define REAL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  float *samples;
  size_t num_samples;
  uint32_t sample_rate_Hz;
} real_data_t;

void destroy_real_data(real_data_t *real_data);

#endif // REAL_H
