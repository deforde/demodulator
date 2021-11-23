#include "audio.h"
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

    fir_filter_c_t mono_audio_filter;
    init_filter_c(&mono_audio_filter, FIR_FILT_250kFS_15kPA_19kST, N_FIR_FILT_250kFS_15kPA_19kST);

    uint32_t decimation_factor = 1;
    iq_data_t filtered_iq_data;
    filtered_iq_data.sample_rate_Hz = iq_data.sample_rate_Hz / decimation_factor;

    apply_filter_c(&mono_audio_filter, decimation_factor, iq_data.samples, iq_data.num_samples, &filtered_iq_data.samples, &filtered_iq_data.num_samples);
    destroy_filter_c(&mono_audio_filter);

    destroy_iq_data(&iq_data);

    audio_data_t audio_data;
    audio_data.sample_rate_Hz = filtered_iq_data.sample_rate_Hz;

    polar_discriminant(filtered_iq_data.samples, filtered_iq_data.num_samples, &audio_data.samples, &audio_data.num_samples);

    destroy_iq_data(&filtered_iq_data);

    fir_filter_r_t audio_filter;
    init_filter_r(&audio_filter, FIR_FILT_250kFS_25kPA_32kST, N_FIR_FILT_250kFS_25kPA_32kST);

    decimation_factor = 4;
    audio_data_t filtered_audio_data;
    filtered_audio_data.sample_rate_Hz = audio_data.sample_rate_Hz / decimation_factor;

    apply_filter_r(&audio_filter, decimation_factor, audio_data.samples, audio_data.num_samples, &filtered_audio_data.samples, &filtered_audio_data.num_samples);
    destroy_filter_r(&audio_filter);

    destroy_audio_data(&audio_data);

    FILE* fp = fopen("audio.bin", "wb");
    if(!fp) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
    } else {
        fwrite(filtered_audio_data.samples, 1, filtered_audio_data.num_samples * sizeof(float), fp);
        fclose(fp);
    }

    destroy_audio_data(&filtered_audio_data);

    return EXIT_SUCCESS;
}
