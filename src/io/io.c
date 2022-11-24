#include "io.h"

#include <stdio.h>

bool write_data_to_file(const void *data, size_t size, const char *filename) {
  FILE *fp = fopen(filename, "ab");

  if (!fp) {
    fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
    return false;
  }

  fwrite(data, 1, size, fp);
  fclose(fp);

  return true;
}
