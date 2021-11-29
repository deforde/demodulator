#include "interconnect.h"

void init_interconnect(interconnect_t* const interconnect)
{
    pthread_rwlock_init(&interconnect->rw_lock, NULL);
    pthread_cond_init(&interconnect->cond, NULL);
    pthread_mutex_init(&interconnect->mx, NULL);
}

void destroy_interconnect(interconnect_t* const interconnect)
{
    pthread_rwlock_destroy(&interconnect->rw_lock);
    pthread_cond_destroy(&interconnect->cond);
    pthread_mutex_destroy(&interconnect->mx);
}

void lock_read(interconnect_t* const interconnect)
{
    pthread_rwlock_rdlock(&interconnect->rw_lock);
}

void lock_write(interconnect_t* const interconnect)
{
    pthread_rwlock_wrlock(&interconnect->rw_lock);
}

void unlock_rw(interconnect_t* const interconnect)
{
    pthread_rwlock_unlock(&interconnect->rw_lock);
}

void wait(interconnect_t* const interconnect)
{
    pthread_cond_wait(&interconnect->cond, &interconnect->mx);
}

void signal(interconnect_t* const interconnect)
{
    pthread_cond_signal(&interconnect->cond);
}
