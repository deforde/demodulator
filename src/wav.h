#ifndef WAV_H
#define WAV_H

#include "worker.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct
{
    char riff[4];
    uint32_t file_size;
    char wave[4];
    char fmt[4];
    uint32_t fmt_data_len;
    uint16_t fmt_type;
    uint16_t num_channels;
    uint32_t sample_rate_Hz;
    uint32_t bytes_per_sec;
    uint16_t channel_bytes_per_sample;
    uint16_t bits_per_sample;
} wav_header_t;

typedef struct
{
    char chunk_name[4];
    uint32_t chunk_len;
} chunk_header_t;
#pragma pack(pop)

typedef struct
{
    const char* filename;
    worker_t worker;
} wav_file_reader_t;

void init_wav_file_reader(wav_file_reader_t* wav_file_reader, interconnect_t* output, const char** filename);
void destroy_wav_file_reader(wav_file_reader_t* wav_file_reader);

#endif //WAV_H
