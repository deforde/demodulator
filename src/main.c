#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "concurrency/worker.h"
#include "fm/fm.h"
#include "io/wav.h"

void usage() {
  printf("usage: demodulator -f file -m modulation\n"
         "\t-f, --file source_data_file         A WAVE file containing the "
         "modulated source data\n"
         "\t-m, --modulation modulation_type    The type of modulation applied "
         "to the data contained in the source file (default: fm)\n"
         "\t                                    Supported: fm\n"
         "\t-h, --help                          Print usage and exit\n"
         "\n");
}

int main(int argc, char *argv[]) {
  char filename[PATH_MAX] = {0};
  char modulation_type[PATH_MAX] = {0};
  int opt = 0;
  struct option longopts[] = {{"help", no_argument, NULL, 'h'},
                              {"file", required_argument, NULL, 'f'},
                              {"modulation", required_argument, NULL, 'm'},
                              {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "hf:m:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'f':
      strncpy(filename, optarg, sizeof(filename));
      filename[sizeof(filename) - 1] = '\0';
      break;
    case 'm':
      strncpy(modulation_type, optarg, sizeof(modulation_type));
      modulation_type[sizeof(modulation_type) - 1] = '\0';
      break;
    case 'h':
      usage();
      return EXIT_SUCCESS;
    case '?':
    case ':':
    default:
      usage();
      return EXIT_FAILURE;
    }
  }

  if (filename[0] == 0) {
    fprintf(stderr, "No filename provided.\n");
    usage();
    return EXIT_FAILURE;
  }

  if (modulation_type[0] == 0) {
    fprintf(stderr, "No modulation type specified.\n");
    usage();
    return EXIT_FAILURE;
  }

  if (strncmp(modulation_type, "fm", 2) != 0) {
    fprintf(stderr,
            "Illegal value: \"%s\" specified for modulation type. Currently "
            "only \"fm\" is supported.\n",
            modulation_type);
    return EXIT_FAILURE;
  }

  wav_file_reader_t wav_file_reader;
  fm_demod_t fm_demod;

  init_fm_demod(&fm_demod, NULL);
  init_wav_file_reader(&wav_file_reader, &fm_demod.worker.input, filename);

  start_worker(&fm_demod.worker);
  start_worker(&wav_file_reader.worker);

  join_worker(&wav_file_reader.worker);
  join_worker(&fm_demod.worker);

  destroy_wav_file_reader(&wav_file_reader);
  destroy_fm_demod(&fm_demod);

  return EXIT_SUCCESS;
}
