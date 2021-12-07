#ifndef RDS_H
#define RDS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t block;
    bool syncd;
    size_t num_bits_since_sync;
} rds_interpreter_t;

void init_rds_interpreter(rds_interpreter_t* interpreter);
void interpret_rds_data(rds_interpreter_t* interpreter, uint32_t bit);
void destroy_rds_interpreter(rds_interpreter_t* interpreter);

#endif //RDS_H
