#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef void* (*thread_func_t)(void*);

typedef struct {
    thread_func_t func;
    void* args;
} thread_args_wrapper_t;

typedef struct {
    pthread_attr_t attr;
    pthread_t handle;
    thread_args_wrapper_t args;
} thread_t;

void launch_thread(thread_t* thread, thread_func_t func, void* thread_args);

void join_thread(thread_t* thread);

#endif //THREAD_H
