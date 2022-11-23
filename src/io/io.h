#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stddef.h>

bool write_data_to_file(const void *data, size_t size, const char *filename);

#endif // IO_H
