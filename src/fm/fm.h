#ifndef FM_H
#define FM_H

#include <complex.h>
#include <stddef.h>

void polar_discriminant(const float complex* input, size_t input_len, float** output, size_t* output_len);

#endif //FM_H
