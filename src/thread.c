#include "thread.h"

void* thread_impl(void* args)
{
    thread_args_wrapper_t* args_wrapper = (thread_args_wrapper_t*)args;
    void* p_args = args_wrapper->args;

    thread_func_t func = args_wrapper->func;

    void* retval = func(p_args);

    pthread_exit(retval);
}

void launch_thread(thread_t* thread, thread_func_t func, void* thread_args)
{
    thread->args.func = func;
    thread->args.args = thread_args;

    pthread_attr_init(&thread->attr);
    pthread_attr_setdetachstate(&thread->attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&thread->handle, &thread->attr, thread_impl, (void*)&thread->args);
    pthread_attr_destroy(&thread->attr);
}

void join_thread(thread_t* thread)
{
    void* thread_status;
    pthread_join(thread->handle, &thread_status);
}
