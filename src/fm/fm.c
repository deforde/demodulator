#include "fm.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../concurrency/interconnect.h"
#include "../concurrency/worker.h"
#include "../dsp/fir_filter.h"
#include "../types/iq.h"
#include "../types/real.h"

#define PI 3.14159265359F

static const float FIR_FILT_250kFS_15kPA_19kST[] = { 0.000044219730495340614F, 0.00009440017413002246F, 0.00018365481566381955F, 0.000314099657926496F, 0.0004888737347517136F, 0.0007052772638324323F, 0.0009530618154668994F, 0.001213501937310202F, 0.0014596543220620815F, 0.0016580911975254515F, 0.001772185264006052F, 0.0017667596615661733F, 0.0016136092510598552F, 0.001297190984372922F, 0.0008195541472049096F, 0.00020354235282485188F, -0.0005066031610256816F, -0.0012477912293477878F, -0.001943943396882769F, -0.00251478878088645F, -0.0028866415213852177F, -0.0030037479174223553F, -0.0028384913862145285F, -0.0023986640082025623F, -0.0017302297502465027F, -0.0009145766054104768F, -0.00006000902620814281F, 0.0007118808025280118F, 0.001283360665884154F, 0.0015588594476816553F, 0.001481605191451303F, 0.0010457625891767785F, 0.000301574090541716F, -0.0006479550570090435F, -0.0016591971640395236F, -0.0025677034644882584F, -0.0032131562544198277F, -0.003465828593184935F, -0.0032503729137945982F, -0.002562689289902561F, -0.0014764994872995415F, -0.00013761807392919704F, 0.0012542869500616768F, 0.0024743875585363107F, 0.0033080964852317337F, 0.0035876984968087194F, 0.0032243131182875307F, 0.0022293060231395614F, 0.0007207561105883507F, -0.0010874563644707642F, -0.0029135440066924215F, -0.004449451207866056F, -0.005410406821700571F, -0.005584735973844371F, -0.004875586071167914F, -0.0033268844512126033F, -0.0011278522188292914F, 0.0014064950686490984F, 0.00387759706914003F, 0.005863192233064355F, 0.0069862331966464705F, 0.006982135774058983F, 0.005753020181664978F, 0.00339881578041101F, 0.00021810770475836572F, -0.0033240087439066236F, -0.006658563768166884F, -0.009198145015204788F, -0.010433066286096221F, -0.010023483549336699F, -0.00787209465450969F, -0.004163924710020362F, 0.0006360702272000808F, 0.005831057067516668F, 0.010582242431390191F, 0.014030521700937876F, 0.01543312771302555F, 0.01429615792087495F, 0.010481576834571455F, 0.0042709935291500465F, -0.003627267347436748F, -0.012133396981147423F, -0.019910142427153145F, -0.025523793696773867F, -0.027632445479650606F, -0.025176691782452826F, -0.017546427721962674F, -0.004699384557322631F, 0.012787565557979339F, 0.033744553187915624F, 0.056512266250085125F, 0.07911237948405572F, 0.09946490871347209F, 0.11562550933926899F, 0.12601248946447888F, 0.1295942215312001F, 0.12601248946447888F, 0.11562550933926899F, 0.09946490871347209F, 0.07911237948405572F, 0.056512266250085125F, 0.033744553187915624F, 0.012787565557979339F, -0.004699384557322631F, -0.017546427721962674F, -0.025176691782452826F, -0.027632445479650606F, -0.025523793696773867F, -0.019910142427153145F, -0.012133396981147423F, -0.003627267347436748F, 0.0042709935291500465F, 0.010481576834571455F, 0.01429615792087495F, 0.01543312771302555F, 0.014030521700937876F, 0.010582242431390191F, 0.005831057067516668F, 0.0006360702272000808F, -0.004163924710020362F, -0.00787209465450969F, -0.010023483549336699F, -0.010433066286096221F, -0.009198145015204788F, -0.006658563768166884F, -0.0033240087439066236F, 0.00021810770475836572F, 0.00339881578041101F, 0.005753020181664978F, 0.006982135774058983F, 0.0069862331966464705F, 0.005863192233064355F, 0.00387759706914003F, 0.0014064950686490984F, -0.0011278522188292914F, -0.0033268844512126033F, -0.004875586071167914F, -0.005584735973844371F, -0.005410406821700571F, -0.004449451207866056F, -0.0029135440066924215F, -0.0010874563644707642F, 0.0007207561105883507F, 0.0022293060231395614F, 0.0032243131182875307F, 0.0035876984968087194F, 0.0033080964852317337F, 0.0024743875585363107F, 0.0012542869500616768F, -0.00013761807392919704F, -0.0014764994872995415F, -0.002562689289902561F, -0.0032503729137945982F, -0.003465828593184935F, -0.0032131562544198277F, -0.0025677034644882584F, -0.0016591971640395236F, -0.0006479550570090435F, 0.000301574090541716F, 0.0010457625891767785F, 0.001481605191451303F, 0.0015588594476816553F, 0.001283360665884154F, 0.0007118808025280118F, -0.00006000902620814281F, -0.0009145766054104768F, -0.0017302297502465027F, -0.0023986640082025623F, -0.0028384913862145285F, -0.0030037479174223553F, -0.0028866415213852177F, -0.00251478878088645F, -0.001943943396882769F, -0.0012477912293477878F, -0.0005066031610256816F, 0.00020354235282485188F, 0.0008195541472049096F, 0.001297190984372922F, 0.0016136092510598552F, 0.0017667596615661733F, 0.001772185264006052F, 0.0016580911975254515F, 0.0014596543220620815F, 0.001213501937310202F, 0.0009530618154668994F, 0.0007052772638324323F, 0.0004888737347517136F, 0.000314099657926496F, 0.00018365481566381955F, 0.00009440017413002246F, 0.000044219730495340614F };
static const float FIR_FILT_250kFS_100kPA_105kST[] = { 0.006070605487454018F, 0.01443402125039274F, 0.003957176954417116F, -0.006535244989599065F, 0.0031187571195364274F, 0.0008788288645458518F, -0.003152063493691762F, 0.0033074824188423385F, -0.0019563566347321594F, -0.0000025383003585240167F, 0.0016799331349582338F, -0.002575570344332358F, 0.00241397113454167F, -0.001397456697452616F, -0.00011472040180456252F, 0.0015983789247068835F, -0.002512407806746633F, 0.0025711208951452666F, -0.0017085805309174612F, 0.0001728688337901377F, 0.001522693696331991F, -0.0027756503415741125F, 0.003139961991236923F, -0.0023877168791097054F, 0.0006911070754891852F, 0.0013954938375801867F, -0.0031639324578624805F, 0.003946348838187892F, -0.003370945227201318F, 0.0015043655171701233F, 0.0010929587055851092F, -0.003540088206073499F, 0.004936743470639754F, -0.004686088584383647F, 0.0027040886187351806F, 0.0004727755075597306F, -0.0037923630167455685F, 0.006069663758519273F, -0.00637512086870205F, 0.004401003996342946F, -0.0006286072117454427F, -0.0037692431030611163F, 0.007271847207576821F, -0.008509681144083387F, 0.006790694495379134F, -0.0024281973430471883F, -0.0032954149989525695F, 0.008471502086090477F, -0.011181525650613497F, 0.01015676206577768F, -0.005315208528685315F, -0.00205813889816767F, 0.009619887176772717F, -0.01466953669794694F, 0.015051233680384236F, -0.00998261729272495F, 0.0005146939348710277F, 0.01060827917417486F, -0.019635329327555812F, 0.02294867703898887F, -0.01832826815369223F, 0.00592488168896409F, 0.011383598799338635F, -0.028448255024472447F, 0.03904460237343456F, -0.03753170300608063F, 0.02052489922392698F, 0.011868484728511467F, -0.055678532884019305F, 0.10370293145139495F, -0.1470442752839363F, 0.1771743251172417F, 0.8120392115232855F, 0.1771743251172417F, -0.1470442752839363F, 0.10370293145139495F, -0.055678532884019305F, 0.011868484728511467F, 0.02052489922392698F, -0.03753170300608063F, 0.03904460237343456F, -0.028448255024472447F, 0.011383598799338635F, 0.00592488168896409F, -0.01832826815369223F, 0.02294867703898887F, -0.019635329327555812F, 0.01060827917417486F, 0.0005146939348710277F, -0.00998261729272495F, 0.015051233680384236F, -0.01466953669794694F, 0.009619887176772717F, -0.00205813889816767F, -0.005315208528685315F, 0.01015676206577768F, -0.011181525650613497F, 0.008471502086090477F, -0.0032954149989525695F, -0.0024281973430471883F, 0.006790694495379134F, -0.008509681144083387F, 0.007271847207576821F, -0.0037692431030611163F, -0.0006286072117454427F, 0.004401003996342946F, -0.00637512086870205F, 0.006069663758519273F, -0.0037923630167455685F, 0.0004727755075597306F, 0.0027040886187351806F, -0.004686088584383647F, 0.004936743470639754F, -0.003540088206073499F, 0.0010929587055851092F, 0.0015043655171701233F, -0.003370945227201318F, 0.003946348838187892F, -0.0031639324578624805F, 0.0013954938375801867F, 0.0006911070754891852F, -0.0023877168791097054F, 0.003139961991236923F, -0.0027756503415741125F, 0.001522693696331991F, 0.0001728688337901377F, -0.0017085805309174612F, 0.0025711208951452666F, -0.002512407806746633F, 0.0015983789247068835F, -0.00011472040180456252F, -0.001397456697452616F, 0.00241397113454167F, -0.002575570344332358F, 0.0016799331349582338F, -0.0000025383003585240167F, -0.0019563566347321594F, 0.0033074824188423385F, -0.003152063493691762F, 0.0008788288645458518F, 0.0031187571195364274F, -0.006535244989599065F, 0.003957176954417116F, 0.01443402125039274F, 0.006070605487454018F };

void convert_data_ftos(const float* const input, int16_t* const output, size_t len)
{
    for(size_t i = 0; i < len; ++i) {
        output[i] = input[i] * INT16_MAX;
    }
}

bool write_data_to_file(const void* const data, size_t size, const char* const filename)
{
    FILE* fp = fopen(filename, "ab");

    if(!fp) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
        return false;
    }

    fwrite(data, 1, size, fp);
    fclose(fp);

    return true;
}

void deemphasis_filtering(const float* const input, size_t input_len, float** const output, size_t* output_len, float* const prev_deemph_input, float* const prev_deemph_output)
{
    //           w_ca         1                 1 - (-1) z^-1
    //    H(z) = ---- * ----------- * --------------------------------
    //           2 fs        -w_ca                   -w_ca
    //                   1 - -----               1 + -----
    //                        2 fs                    2 fs
    //                                     1 - -------------- z^-1
    //                                               -w_ca
    //                                           1 - -----
    //                                                2 fs
    //            Y(z)
    //    H(z) = ------
    //            X(z)
    //
    //    Y(z) = k_3 + k_3 * z^-1
    //    X(x) = k_4 + k_5 * z^-1
    //    (k_4 + k_5 * z^-1)Y(z) = (k_3 + k_3 * z^-1)X(z)
    //    y[n] = k_3/k_4 * x[n] + k_3/k_4 * x[n-1] - k_5/k_4 * y[n-1]
    //
    //    let tau = 75 us
    //    let w_ca = tau^-1
    //    let fs = 44.1 kHz
    //    let k_1 = k_3/k_4 = w_ca / (2fs - w_ca)
    //    let k_2 = k_5/k_4 = ((2fs + w_ca)(fs - w_ca)) / ((2fs - w_ca)(fs + w_ca))
    //
    static const float k_1 = 0.17809439002671415F;
    static const float k_2 = 0.726501592564895F;

    *output_len = input_len;
    float* output_buffer = (float*)malloc(*output_len * sizeof(float));
    *output = output_buffer;

    for(size_t i = 0; i < *output_len; ++i) {
        output_buffer[i] = k_1 * input[i] + k_1 * *prev_deemph_input - k_2 * *prev_deemph_output;
        *prev_deemph_input = input[i];
        *prev_deemph_output = output_buffer[i];
    }
}

void polar_discriminant(const float complex* const input, size_t input_len, float** output, size_t* output_len, float complex* const prev_sample)
{
    *output_len = input_len;
    float* output_buffer = (float*)malloc(*output_len * sizeof(float));
    *output = output_buffer;

    for(size_t i = 0; i < input_len; ++i) {
        output_buffer[i] = cargf(input[i] * conjf(*prev_sample)) / PI;
        *prev_sample = input[i];
    }
}

void extract_mono_audio(const real_data_t* const demodulated_data, fir_filter_r_t* const audio_filter, float** const output, size_t* output_len, float* const prev_deemph_input, float* const prev_deemph_output)
{
    if(demodulated_data->sample_rate_Hz != 250000) {
        fprintf(stderr, "Demodulated source data sample rate is expected to be 250kHz, not %uHz!", demodulated_data->sample_rate_Hz); //TODO: Handle arbitrary input sample rates
    }

    // Upsample the source data by a factor of three, this will facilitate subsequent downsampling by an integer value in order to arrive at an output sample rate of 44.1kHz
    const uint32_t interpolation_factor = 3;
    real_data_t upsampled_data;
    upsampled_data.sample_rate_Hz = demodulated_data->sample_rate_Hz * interpolation_factor;
    upsampled_data.num_samples = demodulated_data->num_samples * interpolation_factor;
    upsampled_data.samples = (float*)malloc(upsampled_data.num_samples * sizeof(float));
    memset(upsampled_data.samples, 0, upsampled_data.num_samples * sizeof(float));
    for(size_t i = 0; i < upsampled_data.num_samples; i += interpolation_factor) {
        upsampled_data.samples[i] = demodulated_data->samples[i / interpolation_factor];
    }

    // Decimate by 17 in order to arrive at the final output sample rate of 44.1kHz
    const uint32_t decimation_factor = 17;
    static float hist[16];
    static size_t num_hist_samples = 0;
    // TODO: This should really be part of the fir filter structure itself
    const size_t total_num_samples = (num_hist_samples + upsampled_data.num_samples) / decimation_factor * decimation_factor;
    const size_t samples_to_carry_over = (num_hist_samples + upsampled_data.num_samples) % decimation_factor;
    float* accumulated_data = (float*)malloc(total_num_samples * sizeof(float));
    memcpy(accumulated_data, hist, num_hist_samples * sizeof(float));
    memcpy(accumulated_data + num_hist_samples, upsampled_data.samples, (total_num_samples - num_hist_samples) * sizeof(float));
    memcpy(hist, &upsampled_data.samples[upsampled_data.num_samples - samples_to_carry_over], samples_to_carry_over * sizeof(float));
    num_hist_samples = samples_to_carry_over;

    real_data_t mono_audio_data;
    mono_audio_data.sample_rate_Hz = upsampled_data.sample_rate_Hz / decimation_factor;

    apply_filter_r(audio_filter, decimation_factor, accumulated_data, total_num_samples, &mono_audio_data.samples, &mono_audio_data.num_samples);

    free(accumulated_data);
    destroy_real_data(&upsampled_data);

    // Apply deemphasis filter in order to compensate for the pre-emphasis performed on the transmit side
    deemphasis_filtering(mono_audio_data.samples, mono_audio_data.num_samples, output, output_len, prev_deemph_input, prev_deemph_output);
    destroy_real_data(&mono_audio_data);
}

void* demodulate_fm(void* args)
{
    fm_demod_t* demod = (fm_demod_t*)args;
    worker_t* worker = &demod->worker;
    fir_filter_c_t* input_filter = &demod->input_filter;
    fir_filter_r_t* audio_filter = &demod->audio_filter;
    float complex* polar_discrim_prev_sample = &demod->polar_discrim_prev_sample;
    float* prev_deemph_input = &demod->prev_deemph_input;
    float* prev_deemph_output = &demod->prev_deemph_output;

    bool running = true;
    while(running) {
        iq_data_t* iq_data;

        running = worker_read_input(worker, (void**)&iq_data);

        if(!running) {
            break;
        }

        if(iq_data->sample_rate_Hz != 250000) {
            fprintf(stderr, "Source IQ data sample rate is expected to be 250kHz, not %uHz!", iq_data->sample_rate_Hz); //TODO: Handle arbitrary input sample rates
        }

        // Filter the source IQ data using a LPF with a 100kHz cut-off
        iq_data_t filtered_iq_data;
        filtered_iq_data.sample_rate_Hz = iq_data->sample_rate_Hz;
        apply_filter_c(input_filter, 1, iq_data->samples, iq_data->num_samples, &filtered_iq_data.samples, &filtered_iq_data.num_samples);

        destroy_iq_data(iq_data);
        free(iq_data);

        // Demodulate the FM signal
        real_data_t demodulated_data;
        demodulated_data.sample_rate_Hz = filtered_iq_data.sample_rate_Hz;
        polar_discriminant(filtered_iq_data.samples, filtered_iq_data.num_samples, &demodulated_data.samples, &demodulated_data.num_samples, polar_discrim_prev_sample);

        destroy_iq_data(&filtered_iq_data);

        // Extract the mono audio data
        float* mono_audio = NULL;
        size_t num_mono_samples = 0;
        extract_mono_audio(&demodulated_data, audio_filter, &mono_audio, &num_mono_samples, prev_deemph_input, prev_deemph_output);

        destroy_real_data(&demodulated_data);

        int16_t* mono_audio_16 = (int16_t*)malloc(num_mono_samples * sizeof(int16_t));
        convert_data_ftos(mono_audio, mono_audio_16, num_mono_samples);
        free(mono_audio);

        write_data_to_file(mono_audio_16, num_mono_samples * sizeof(int16_t), "mono_audio.bin");
        free(mono_audio_16);
    }

    return NULL;
}

void init_fm_demod(fm_demod_t* demod, interconnect_t* output)
{
    demod->polar_discrim_prev_sample = 0.0F + 0.0F * I;
    demod->prev_deemph_input = 0.0F;
    demod->prev_deemph_output = 0.0F;

    init_filter_c(&demod->input_filter, FIR_FILT_250kFS_100kPA_105kST, sizeof(FIR_FILT_250kFS_100kPA_105kST) / sizeof(*FIR_FILT_250kFS_100kPA_105kST));
    init_filter_r(&demod->audio_filter, FIR_FILT_250kFS_15kPA_19kST, sizeof(FIR_FILT_250kFS_15kPA_19kST) / sizeof(*FIR_FILT_250kFS_15kPA_19kST));

    init_worker(&demod->worker, output, demodulate_fm, demod);
}

void destroy_fm_demod(fm_demod_t* demod)
{
    destroy_worker(&demod->worker);

    destroy_filter_c(&demod->input_filter);
    destroy_filter_r(&demod->audio_filter);
}
