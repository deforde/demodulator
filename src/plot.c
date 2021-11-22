#include "plot.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISPLAY_WIDTH 202
#define DISPLAY_HEIGHT 46
#define Y_AXIS_LABEL_LEN 7
#define ROW_TRAILER_LEN 1
#define NUM_COLUMNS (Y_AXIS_LABEL_LEN + DISPLAY_WIDTH + ROW_TRAILER_LEN)

float get_min_ampl(float const* ampl, size_t len)
{
    float min_ampl = FLT_MAX;
    for(size_t i = 0; i < len; ++i) {
        min_ampl = fminf(min_ampl, ampl[i]);
    }
    return min_ampl;
}

float get_max_ampl(float const* ampl, size_t len)
{
    float max_ampl = -FLT_MAX;
    for(size_t i = 0; i < len; ++i) {
        max_ampl = fmaxf(max_ampl, ampl[i]);
    }
    return max_ampl;
}

float peak_hold(float const* ampl, size_t len)
{
    float peak = -FLT_MAX;
    for(size_t i = 0; i < len; ++i) {
        peak = fmaxf(peak, ampl[i]);
    }
    return peak;
}

float mean(float const* ampl, size_t len)
{
    float peak = 0.0F;
    for(size_t i = 0; i < len; ++i) {
        peak += ampl[i];
    }
    return peak / len;
}

void plot_amplitude_spectrum(float* ampl, size_t len)
{
    if(DISPLAY_WIDTH > len) {
        //TODO: Provide implementation for case in which the source data len is less than the display width
        fprintf(stderr, "Implementation for case in which the source data len (%lu) is less than the display width (%u) does not yet exist!\n", len, DISPLAY_WIDTH);
        return;
    }

    const double resample_ratio = (double)len / DISPLAY_WIDTH;

    float resampled_data[DISPLAY_WIDTH];
    for(size_t i = 0; i < DISPLAY_WIDTH; ++i) {
        const size_t input_start_index = (size_t)fmin(floor(i * resample_ratio), (double)(len - 1));
        const size_t input_end_index = (size_t)fmin(ceil((double)(i + 1) * resample_ratio), (double)len);
        const size_t input_block_len = input_end_index - input_start_index;

        resampled_data[i] = peak_hold(&ampl[input_start_index], input_block_len);
    }

    static float min_ampl = FLT_MAX;
    static float max_ampl = -FLT_MAX;

    const float data_min_ampl = get_min_ampl(resampled_data, DISPLAY_WIDTH);
    const float data_max_ampl = get_max_ampl(resampled_data, DISPLAY_WIDTH);

    min_ampl = fminf(min_ampl, data_min_ampl);
    max_ampl = fmaxf(max_ampl, data_max_ampl);

    const float ampl_range = max_ampl - min_ampl;

    uint32_t discretised_data[DISPLAY_WIDTH];
    for(size_t i = 0; i < DISPLAY_WIDTH; ++i) {
        discretised_data[i] = (uint32_t)fminf(roundf(((resampled_data[i] - min_ampl) / ampl_range) * DISPLAY_HEIGHT), DISPLAY_HEIGHT - 1);
    }

    char display_grid[DISPLAY_HEIGHT * NUM_COLUMNS + 1]; // + 1 for trailing \0
    float discretized_level = max_ampl;
    float discretized_ampl_res = ampl_range / DISPLAY_HEIGHT;
    for(size_t i = 0; i < DISPLAY_HEIGHT; ++i) {
        int bytes_written = snprintf(&display_grid[i * NUM_COLUMNS], Y_AXIS_LABEL_LEN, "%+.2f", discretized_level);
        discretized_level -= discretized_ampl_res;
        memset(&display_grid[i * NUM_COLUMNS + bytes_written - 1], ' ', Y_AXIS_LABEL_LEN - bytes_written + 1);
        memset(&display_grid[i * NUM_COLUMNS + Y_AXIS_LABEL_LEN], ' ', NUM_COLUMNS - ROW_TRAILER_LEN - Y_AXIS_LABEL_LEN);
        display_grid[i * NUM_COLUMNS + NUM_COLUMNS - 1] = '\n';
    }
    display_grid[DISPLAY_HEIGHT * NUM_COLUMNS] = 0;
    for(size_t i = 0; i < DISPLAY_WIDTH; ++i) {
        const uint32_t ampl = discretised_data[i];
        const size_t row_index = DISPLAY_HEIGHT - 1 - ampl;
        display_grid[row_index * NUM_COLUMNS + i + Y_AXIS_LABEL_LEN] = '-';
    }
    printf("%s", display_grid);
}
