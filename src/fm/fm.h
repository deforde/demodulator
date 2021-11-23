#ifndef FM_H
#define FM_H

#include "../iq.h"

#include <complex.h>
#include <stddef.h>

void demodulate_fm(const iq_data_t* iq_data);

#endif //FM_H
