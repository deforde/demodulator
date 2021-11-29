#include "wav.h"

#include "iq.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* extract_sample_data(void* args)
{
    wav_file_reader_t* wav_file_reader = (wav_file_reader_t*)args;
    worker_t* worker = &wav_file_reader->worker;

    FILE* fp = fopen(wav_file_reader->filename, "r");
    if(!fp) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", wav_file_reader->filename);
        return NULL;
    }

    size_t total_bytes_read = 0;

    wav_header_t wav_header;
    size_t bytes_read = fread(&wav_header, 1, sizeof(wav_header_t), fp);
    if(bytes_read != sizeof(wav_header_t)) {
        fclose(fp);
        fprintf(stderr, "Failed to read the wav file header.\n");
        return NULL;
    }

    size_t total_bytes_expected = wav_header.file_size + ((uint64_t)(&wav_header.file_size) - (uint64_t)(&wav_header) + sizeof(wav_header.file_size));
    total_bytes_read += bytes_read;

    while(total_bytes_read < total_bytes_expected) {
        chunk_header_t chunk_header;
        bytes_read = fread(&chunk_header, 1, sizeof(chunk_header_t), fp);
        if(bytes_read != sizeof(chunk_header_t)) {
            fprintf(stderr, "Failed to read the requisite number of bytes.\n");
            break;
        }

        total_bytes_read += bytes_read;

        if(strncmp(chunk_header.chunk_name, "data", sizeof(chunk_header.chunk_name)) == 0) {
            static const size_t sub_chunk_num_samples = 16388; //TODO: Bit of a hack here, at the moment this needs to be an integer multiple of the decimation factor applied in the FM demodulator due to lack of sample history being maintained there
            static const size_t sample_size = sizeof(int16_t) * 2;
            static const size_t sub_chunk_size = sub_chunk_num_samples * sample_size;

            const size_t num_sub_chunks = (size_t)ceilf((float)chunk_header.chunk_len / sub_chunk_size);
            size_t chunk_data_remaining = chunk_header.chunk_len;

            for(size_t i = 0; i < num_sub_chunks; ++i) {
                const size_t size_to_read = chunk_data_remaining < sub_chunk_size ? chunk_data_remaining : sub_chunk_size;

                uint8_t* buffer = (uint8_t*)malloc(size_to_read);
                bytes_read = fread(buffer, 1, size_to_read, fp);
                if(bytes_read != size_to_read) {
                    fprintf(stderr, "Failed to read the requisite number of bytes.\n");
                    break;
                }

                total_bytes_read += bytes_read;
                chunk_data_remaining -= size_to_read;

                void* iq_struct_buf = malloc(sizeof(iq_data_t));
                iq_data_t* iq_data = (iq_data_t*)iq_struct_buf;
                iq_data->samples = NULL;
                iq_data->num_samples = 0;
                iq_data->sample_rate_Hz = wav_header.sample_rate_Hz;

                const size_t num_samples = bytes_read / sample_size;
                iq_data->samples = (float complex*)malloc(num_samples * sizeof(*iq_data->samples));

                int16_t const* source_samples = (int16_t const*)buffer;
                for(size_t i = 0; i < num_samples; ++i) {
                    iq_data->samples[i] = (float)source_samples[i*2] / INT16_MAX + I * (float)source_samples[i*2 + 1] / INT16_MAX;
                }

                iq_data->num_samples = num_samples;

                worker_send_output(worker, &iq_struct_buf);

                free(buffer);
            }
        }
        else {
            fseek(fp, chunk_header.chunk_len, SEEK_CUR);
            total_bytes_read += chunk_header.chunk_len;
        }

        if (ferror(fp)) {
            fprintf(stderr, "I/O error when reading.\n");
            break;
        }
        if (feof(fp)) {
            printf("End of file reached successfully.\n");
            break;
        }
    }

    fclose(fp);

    return NULL;
}

void init_wav_file_reader(wav_file_reader_t* wav_file_reader, interconnect_t* output, const char** const filename)
{
    wav_file_reader->filename = *filename;
    init_worker(&wav_file_reader->worker, output, extract_sample_data, wav_file_reader);
}

void destroy_wav_file_reader(wav_file_reader_t* wav_file_reader)
{
    destroy_worker(&wav_file_reader->worker);
}
