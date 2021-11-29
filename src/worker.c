#include "worker.h"

void init_worker(worker_t* const worker, interconnect_t* const output, thread_func_t func, void* thread_args)
{
    init_interconnect(&worker->input);
    worker->output = output;
    worker->func = func;
    worker->args = thread_args;
}

void destroy_worker(worker_t* const worker)
{
    destroy_interconnect(&worker->input);
}

void start_worker(worker_t* worker)
{
    launch_thread(&worker->thread, worker->func, worker->args);
}

void join_worker(worker_t* worker)
{
    join_thread(&worker->thread);
}

void wait_input(worker_t* worker)
{
    wait(&worker->input);
}

void signal_output(worker_t* worker)
{
    if(worker->output != NULL) {
        signal(worker->output);
    }
}

void lock_input(worker_t* worker)
{
    lock_read(&worker->input);
}

void unlock_input(worker_t* worker)
{
    unlock_rw(&worker->input);
}

void lock_output(worker_t* worker)
{
    if(worker->output != NULL) {
        lock_write(worker->output);
    }
}

void unlock_output(worker_t* worker)
{
    if(worker->output != NULL) {
        unlock_rw(worker->output);
    }
}

void read_input(worker_t* worker, void** data)
{
    wait_input(worker);
    lock_input(worker);
    *data = worker->input.data;
    unlock_input(worker);
}

void send_output(worker_t* worker, void** data)
{
    lock_output(worker);
    worker->output->data = *data;
    unlock_output(worker);
    signal_output(worker);
}
