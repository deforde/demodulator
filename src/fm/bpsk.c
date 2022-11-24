#include "bpsk.h"

#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../decode/diff_man.h"
#include "../interpret/rds.h"
#include "../types/real.h"

static const uint32_t SAMPLES_PER_SYMBOL = 3;
static const float TARGET_SAMPLE_RATE_HZ = 171000.0f;

void init_bpsk_demod(bpsk_demod_t *demod) {
  const float input_sample_rate_Hz = 250000.0f;
  const float carrier_frequency_Hz = 57000.0f;
  const float agc_bandwidth = 500.0f;
  const float agc_initial_gain = 0.08f;
  const float low_pass_cutoff_Hz = 2400.0f;
  const float sym_sync_bandwidth_Hz = 2200.0f;
  const uint32_t sym_sync_delay = 3;
  const float sym_sync_beta = 0.8f;
  const float pll_bandwidth_Hz = 0.01f;

  demod->sample_count = 0;
  demod->resampling_ratio = TARGET_SAMPLE_RATE_HZ / input_sample_rate_Hz;

  demod->resampler =
      resamp_rrrf_create(demod->resampling_ratio, 13, 0.47f, 60.0f, 32);

  demod->filter = firfilt_crcf_create_kaiser(
      255, low_pass_cutoff_Hz / TARGET_SAMPLE_RATE_HZ, 60.0f, 0.0f);
  firfilt_crcf_set_scale(demod->filter, 2.0f * low_pass_cutoff_Hz);

  demod->agc = agc_crcf_create();
  agc_crcf_set_bandwidth(demod->agc, agc_bandwidth / TARGET_SAMPLE_RATE_HZ);
  agc_crcf_set_gain(demod->agc, agc_initial_gain);

  demod->oscillator = nco_crcf_create(LIQUID_NCO);
  nco_crcf_set_frequency(demod->oscillator, 2.0f * M_PI * carrier_frequency_Hz /
                                                TARGET_SAMPLE_RATE_HZ);
  nco_crcf_pll_set_bandwidth(demod->oscillator,
                             pll_bandwidth_Hz / TARGET_SAMPLE_RATE_HZ);

  demod->sym_sync =
      symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC, SAMPLES_PER_SYMBOL,
                                   sym_sync_delay, sym_sync_beta, 32);
  symsync_crcf_set_lf_bw(demod->sym_sync,
                         sym_sync_bandwidth_Hz / TARGET_SAMPLE_RATE_HZ);
  symsync_crcf_set_output_rate(demod->sym_sync, 1);

  demod->modem = modemcf_create(LIQUID_MODEM_PSK2);

  init_diff_man_decoder(&demod->decoder);

  init_rds_interpreter(&demod->rds);
}

void demodulate_bpsk(bpsk_demod_t *demod, const real_data_t *input_data) {
  if (input_data->sample_rate_Hz != 250000) {
    fprintf(stderr,
            "Source IQ data sample rate is expected to be 250kHz, not %uHz!",
            input_data
                ->sample_rate_Hz); // TODO: Handle arbitrary input sample rates
  }

  const float bits_per_second = 1187.5f;
  const float pll_multiplier = 12.0f;

  resamp_rrrf resampler = demod->resampler;
  firfilt_crcf filter = demod->filter;
  agc_crcf agc = demod->agc;
  nco_crcf oscillator = demod->oscillator;
  symsync_crcf sym_sync = demod->sym_sync;
  modemcf modem = demod->modem;
  diff_man_decoder_t *decoder = &demod->decoder;
  rds_interpreter_t *rds = &demod->rds;

  float *resampled_data = NULL;

  resampled_data = (float *)malloc(
      (size_t)ceilf(demod->resampling_ratio * input_data->num_samples) *
      sizeof(float));
  size_t resampled_data_index = 0;

  for (size_t i = 0; i < input_data->num_samples; ++i) {
    unsigned int num_output_samples = 0;
    resamp_rrrf_execute(resampler, input_data->samples[i],
                        &resampled_data[resampled_data_index],
                        &num_output_samples);
    resampled_data_index += num_output_samples;
  }
  const size_t total_num_resampled_samples = resampled_data_index;

  int decimation_ratio =
      (int)(TARGET_SAMPLE_RATE_HZ / bits_per_second / 2 / SAMPLES_PER_SYMBOL);
  for (size_t i = 0; i < total_num_resampled_samples; ++i) {
    liquid_float_complex input = resampled_data[i] + 0.0f * I;
    liquid_float_complex sample_baseband;
    nco_crcf_mix_down(oscillator, input, &sample_baseband);

    firfilt_crcf_push(filter, sample_baseband);

    if (demod->sample_count % decimation_ratio == 0) {
      liquid_float_complex sample_lowpass;
      firfilt_crcf_execute(filter, &sample_lowpass);

      liquid_float_complex sample_agc;
      agc_crcf_execute(agc, sample_lowpass, &sample_agc);

      liquid_float_complex sample_sym_sync;
      unsigned int n_out = 0;
      symsync_crcf_execute(sym_sync, &sample_agc, 1, &sample_sym_sync, &n_out);

      if (n_out > 0) {
        unsigned int symbol_out = 0;
        modemcf_demodulate(modem, sample_sym_sync, &symbol_out);

        float phase_error = modemcf_get_demodulator_phase_error(modem);
        nco_crcf_pll_step(oscillator, phase_error * pll_multiplier);

        const uint32_t symbol_bit = crealf(sample_sym_sync) > 0.0f ? 1 : 0;
        uint32_t data_bit;
        const bool data_bit_produced =
            decode_diff_man(decoder, symbol_bit, &data_bit);
        if (data_bit_produced) {
          interpret_rds_data(rds, data_bit);
        }
      }
    }

    nco_crcf_step(oscillator);

    ++demod->sample_count;
  }

  free(resampled_data);
}

void destroy_bpsk_demod(bpsk_demod_t *demod) {
  demod->sample_count = 0;
  demod->resampling_ratio = 0.0f;

  resamp_rrrf_destroy(demod->resampler);
  firfilt_crcf_destroy(demod->filter);
  agc_crcf_destroy(demod->agc);
  nco_crcf_destroy(demod->oscillator);
  symsync_crcf_destroy(demod->sym_sync);
  modemcf_destroy(demod->modem);

  destroy_diff_man_decoder(&demod->decoder);

  destroy_rds_interpreter(&demod->rds);
}
