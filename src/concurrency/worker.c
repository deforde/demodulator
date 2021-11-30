#include "worker.h"

#include <stddef.h>

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
    if(worker->output) {
        interconnect_close(worker->output);
    }
}

void wait_input(worker_t* worker)
{
    interconnect_wait(&worker->input);
}

void signal_output(worker_t* worker)
{
    interconnect_signal(worker->output);
}

void wait_output(worker_t* worker)
{
    interconnect_wait(worker->output);
}

void signal_input(worker_t* worker)
{
    interconnect_signal(&worker->input);
}

void lock_input(worker_t* worker)
{
    interconnect_lock(&worker->input);
}

void unlock_input(worker_t* worker)
{
    interconnect_unlock(&worker->input);
}

void lock_output(worker_t* worker)
{
    interconnect_lock(worker->output);
}

void unlock_output(worker_t* worker)
{
    interconnect_unlock(worker->output);
}

bool worker_read_input(worker_t* worker, void** data)
{
    bool input_open = true;
    bool data_available = false;
    *data = NULL;

    lock_input(worker);

    // Check whether the input has already indicated that new data is available
    data_available = worker->input.available;
    if(data_available) {
        worker->input.ready = true;
        worker->input.available = false;
        *data = worker->input.data;
        unlock_input(worker);
        // Let the input know that we are ready to receive new data
        signal_input(worker);
        return input_open;
    }

    // If no new data has been received, and the input is closed, then no new data is ever going to be received, so return false
    input_open = worker->input.open;
    if(!input_open) {
        unlock_input(worker);
        return input_open;
    }

    // If the there is no new data on the input, then wait to be signalled
    while(!data_available) {
        wait_input(worker);
        data_available = worker->input.available;
        if(data_available) {
            worker->input.ready = true;
            worker->input.available = false;
            *data = worker->input.data;
        }
        input_open = worker->input.open;
        if(!input_open) {
            unlock_input(worker);
            return input_open;
        }
    }

    unlock_input(worker);
    // Let the input know that we are ready to receive new data
    signal_input(worker);

    return input_open;
}

void worker_send_output(worker_t* worker, void** data)
{
    if(worker->output == NULL) {
        return;
    }

    bool output_ready = false;

    // Check whether the output has already indicated that it is ready to receive data
    lock_output(worker);
    output_ready = worker->output->ready;
    if(output_ready) {
        worker->output->ready = false;
        worker->output->available = true;
        worker->output->data = *data;
        unlock_output(worker);
        // Let the output know that new data is available
        signal_output(worker);
        return;
    }

    // If the output is not ready to receive data, then wait to be signalled
    while(!output_ready) {
        wait_output(worker);
        output_ready = worker->output->ready;
        if(output_ready) {
            worker->output->ready = false;
            worker->output->available = true;
            worker->output->data = *data;
        }
    }

    unlock_output(worker);
    // Let the output know that new data is available
    signal_output(worker);
}
