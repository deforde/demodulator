#include "io/wav.h"
#include "fm/fm.h"

#include <stdlib.h>
#include <string.h>

int main()
{
    const char* filename = "../data/SDRuno_20200907_184033Z_88110kHz.wav";

    wav_file_reader_t wav_file_reader;
    fm_demod_t fm_demod;

    init_fm_demod(&fm_demod, NULL);
    init_wav_file_reader(&wav_file_reader, &fm_demod.worker.input, &filename);

    start_worker(&fm_demod.worker);
    start_worker(&wav_file_reader.worker);

    join_worker(&wav_file_reader.worker);
    join_worker(&fm_demod.worker);

    destroy_wav_file_reader(&wav_file_reader);
    destroy_fm_demod(&fm_demod);

    return EXIT_SUCCESS;
}
