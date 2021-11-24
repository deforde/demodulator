#include "real.h"
#include "fft.h"
#include "fir_filter.h"
#include "fm/fm.h"
#include "iq.h"
#include "plot.h"
#include "wav.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    const char* filename = "../data/SDRuno_20200907_184033Z_88110kHz.wav";

    iq_data_t iq_data;
    const bool result = ExtractSampleData(filename, &iq_data);
    if(!result) {
        return EXIT_FAILURE;
    }

    demodulate_fm(&iq_data);

    destroy_iq_data(&iq_data);

    return EXIT_SUCCESS;
}
