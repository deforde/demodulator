#include "bpsk.h"

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../io/io.h"
#include "../types/real.h"

static const float SAMPLES_PER_SYMBOL = 3.0F;
static const float TARGET_SAMPLE_RATE_HZ = 171000.0F;

void init_bpsk_demod(bpsk_demod_t* demod)
{
    const float input_sample_rate_Hz = 250000.0F;
    const float carrier_frequency_Hz = 57000.0F;
    const float agc_bandwidth = 500.0F;
    const float agc_initial_gain = 0.08F;
    const float low_pass_cutoff_Hz = 2400.0F;
    const float sym_sync_bandwidth_Hz = 2200.0F;
    const float sym_sync_delay = 3.0F;
    const float sym_sync_beta = 0.8F;
    const float pll_bandwidth_Hz = 0.01F;

    demod->sample_count = 0;
    demod->resampling_ratio = TARGET_SAMPLE_RATE_HZ / input_sample_rate_Hz;

    demod->resampler = resamp_rrrf_create(demod->resampling_ratio, 13, 0.47F, 60.0F, 32);

    demod->filter = firfilt_crcf_create_kaiser(255, low_pass_cutoff_Hz / TARGET_SAMPLE_RATE_HZ, 60.0F, 0.0F);
    firfilt_crcf_set_scale(demod->filter, 2.0F * low_pass_cutoff_Hz);

    demod->agc = agc_crcf_create();
    agc_crcf_set_bandwidth(demod->agc, agc_bandwidth / TARGET_SAMPLE_RATE_HZ);
    agc_crcf_set_gain(demod->agc, agc_initial_gain);

    demod->oscillator = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_frequency(demod->oscillator, 2.0F * M_PI * carrier_frequency_Hz / TARGET_SAMPLE_RATE_HZ);
    nco_crcf_pll_set_bandwidth(demod->oscillator, pll_bandwidth_Hz / TARGET_SAMPLE_RATE_HZ);

    demod->sym_sync = symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC, SAMPLES_PER_SYMBOL, sym_sync_delay, sym_sync_beta, 32);
    symsync_crcf_set_lf_bw(demod->sym_sync, sym_sync_bandwidth_Hz / TARGET_SAMPLE_RATE_HZ);
    symsync_crcf_set_output_rate(demod->sym_sync, 1);

    demod->modem = modemcf_create(LIQUID_MODEM_PSK2);
}

void demodulate_bpsk(bpsk_demod_t* demod, const real_data_t* const input_data)
{
    if(input_data->sample_rate_Hz != 250000) {
        fprintf(stderr, "Source IQ data sample rate is expected to be 250kHz, not %uHz!", input_data->sample_rate_Hz); //TODO: Handle arbitrary input sample rates
    }

    const float bits_per_second = 1187.5F;
    const float pll_multiplier = 12.0F;

    resamp_rrrf resampler = demod->resampler;
    firfilt_crcf filter = demod->filter;
    agc_crcf agc = demod->agc;
    nco_crcf oscillator = demod->oscillator;
    symsync_crcf sym_sync = demod->sym_sync;
    modemcf modem = demod->modem;

    float* resampled_data = NULL;
    liquid_float_complex* symbols = NULL;

    resampled_data = (float*)malloc((size_t)ceilf(demod->resampling_ratio * input_data->num_samples) * sizeof(float));
    size_t resampled_data_index = 0;

    for(size_t i = 0; i < input_data->num_samples; ++i) {
        unsigned int num_output_samples = 0;
        resamp_rrrf_execute(resampler, input_data->samples[i], &resampled_data[resampled_data_index], &num_output_samples);
        resampled_data_index += num_output_samples;
    }
    const size_t total_num_resampled_samples = resampled_data_index;

    int decimation_ratio = TARGET_SAMPLE_RATE_HZ / bits_per_second / 2 / SAMPLES_PER_SYMBOL;
    symbols = (liquid_float_complex*)malloc((size_t)ceilf((float)total_num_resampled_samples / decimation_ratio) * sizeof(liquid_float_complex));
    size_t symbol_index = 0;

    for(size_t i = 0; i < total_num_resampled_samples; ++i) {
        liquid_float_complex input = resampled_data[i] + 0.0F * I;
        liquid_float_complex sample_baseband;
        nco_crcf_mix_down(oscillator, input, &sample_baseband);

        firfilt_crcf_push(filter, sample_baseband);

        if(demod->sample_count % decimation_ratio == 0) {
            liquid_float_complex sample_lowpass;
            firfilt_crcf_execute(filter, &sample_lowpass);

            liquid_float_complex sample_agc;
            agc_crcf_execute(agc, sample_lowpass, &sample_agc);

            liquid_float_complex sample_sym_sync;
            unsigned int n_out = 0;
            symsync_crcf_execute(sym_sync, &sample_agc, 1, &sample_sym_sync, &n_out);

            if(n_out > 0) {
                unsigned int symbol_out = 0;
                modemcf_demodulate(modem, sample_sym_sync, &symbol_out);

                float phase_error = modemcf_get_demodulator_phase_error(modem);
                nco_crcf_pll_step(oscillator, phase_error * pll_multiplier);

                symbols[symbol_index++] = sample_sym_sync;
            }
        }

        nco_crcf_step(oscillator);

        ++demod->sample_count;
    }

    const char* output_file = "bpsk.bin";
    write_data_to_file((void*)symbols, symbol_index * sizeof(*symbols), output_file);

    free(symbols);
    free(resampled_data);
}

void destroy_bpsk_demod(bpsk_demod_t* demod)
{
    demod->sample_count = 0;
    demod->resampling_ratio = 0.0F;

    resamp_rrrf_destroy(demod->resampler);
    firfilt_crcf_destroy(demod->filter);
    agc_crcf_destroy(demod->agc);
    nco_crcf_destroy(demod->oscillator);
    symsync_crcf_destroy(demod->sym_sync);
    modemcf_destroy(demod->modem);
}
