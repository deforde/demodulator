#ifndef WAV_H
#define WAV_H

#include "iq.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct wav_header_t
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

typedef struct chunk_header_t
{
    char chunk_name[4];
    uint32_t chunk_len;
} chunk_header_t;
#pragma pack(pop)

bool ExtractSampleData(const char* filename, iq_data_t* iq_data);

#endif //WAV_H
