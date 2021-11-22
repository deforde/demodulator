#include "wav.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool ExtractSampleData(const char* filename, int16_t** samples, size_t* num_samples)
{
    bool result = false;
    *samples = NULL;
    *num_samples = 0;

    FILE* fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
        return result;
    }

    wav_header_t wav_header;
    size_t bytes_read = fread(&wav_header, 1, sizeof(wav_header_t), fp);
    if(bytes_read != sizeof(wav_header_t)) {
        fclose(fp);
        fprintf(stderr, "Failed to read the wav file header.\n");
        return result;
    }

    for(;;) {
        chunk_header_t chunk_header;
        size_t bytes_read = fread(&chunk_header, 1, sizeof(chunk_header_t), fp);
        if(bytes_read != sizeof(chunk_header_t)) {
            fprintf(stderr, "Failed to read the requisite number of bytes.\n");
            break;
        }

        uint8_t* buffer = (uint8_t*)malloc(chunk_header.chunk_len);
        bytes_read = fread(buffer, 1, chunk_header.chunk_len, fp);
        if(bytes_read != chunk_header.chunk_len) {
            fprintf(stderr, "Failed to read the requisite number of bytes.\n");
            break;
        }

        if(strncmp(chunk_header.chunk_name, "data", sizeof(chunk_header.chunk_name)) == 0) {
            *samples = (int16_t*)buffer;
            *num_samples = chunk_header.chunk_len / sizeof(int16_t);
            result = true;
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

    return result;
}
