#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <pthread.h>

typedef struct {
    pthread_rwlock_t rw_lock;
    pthread_cond_t cond;
    pthread_mutex_t mx;
    void* data;
} interconnect_t;

void init_interconnect(interconnect_t* interconnect);
void destroy_interconnect(interconnect_t* interconnect);

void lock_read(interconnect_t* interconnect);
void lock_write(interconnect_t* interconnect);
void unlock_rw(interconnect_t* interconnect);

void wait(interconnect_t* interconnect);
void signal(interconnect_t* interconnect);

#endif //INTERCONNECT_H
