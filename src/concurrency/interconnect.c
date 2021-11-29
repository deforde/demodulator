#include "interconnect.h"

void init_interconnect(interconnect_t* const interconnect)
{
    interconnect->open = true;
    interconnect->ready = true;
    interconnect->available = false;

    pthread_cond_init(&interconnect->cond, NULL);
    pthread_mutex_init(&interconnect->mx, NULL);
}

void destroy_interconnect(interconnect_t* const interconnect)
{
    interconnect->open = false;
    interconnect->ready = false;
    interconnect->available = false;

    pthread_cond_destroy(&interconnect->cond);
    pthread_mutex_destroy(&interconnect->mx);
}

void interconnect_lock(interconnect_t* const interconnect)
{
    pthread_mutex_lock(&interconnect->mx);
}

void interconnect_unlock(interconnect_t* const interconnect)
{
    pthread_mutex_unlock(&interconnect->mx);
}

void interconnect_wait(interconnect_t* const interconnect)
{
    pthread_cond_wait(&interconnect->cond, &interconnect->mx);
}

void interconnect_signal(interconnect_t* const interconnect)
{
    pthread_cond_signal(&interconnect->cond);
}

void interconnect_close(interconnect_t* interconnect)
{
    interconnect_lock(interconnect);
    interconnect->open = false;
    interconnect_unlock(interconnect);
    interconnect_signal(interconnect);
}
