#ifndef WORKER_H
#define WORKER_H

#include <stdbool.h>

#include "interconnect.h"
#include "thread.h"

typedef struct {
    thread_t thread;
    interconnect_t input;
    interconnect_t* output;
    thread_func_t func;
    void* args;
} worker_t;

void init_worker(worker_t* worker, interconnect_t* output, thread_func_t func, void* thread_args);
void destroy_worker(worker_t* worker);

void start_worker(worker_t* worker);
void join_worker(worker_t* worker);

bool worker_read_input(worker_t* worker, void** data);
void worker_send_output(worker_t* worker, void** data);

#endif //WORKER_H
