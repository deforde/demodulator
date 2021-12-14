#ifndef WAV_H
#define WAV_H

#include <limits.h>

#include "../concurrency/interconnect.h"
#include "../concurrency/worker.h"

typedef struct
{
    char filename[PATH_MAX];
    worker_t worker;
} wav_file_reader_t;

void init_wav_file_reader(wav_file_reader_t* wav_file_reader, interconnect_t* output, const char* filename);
void destroy_wav_file_reader(wav_file_reader_t* wav_file_reader);

#endif //WAV_H
