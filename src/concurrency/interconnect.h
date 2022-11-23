#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <pthread.h>
#include <stdbool.h>

typedef struct {
  bool open;
  bool available;
  bool ready;

  pthread_cond_t cond;
  pthread_mutex_t mx;

  void *data;

} interconnect_t;

void init_interconnect(interconnect_t *interconnect);
void destroy_interconnect(interconnect_t *interconnect);

void interconnect_lock(interconnect_t *interconnect);
void interconnect_unlock(interconnect_t *interconnect);

void interconnect_wait(interconnect_t *interconnect);
void interconnect_signal(interconnect_t *interconnect);

void interconnect_close(interconnect_t *interconnect);

#endif // INTERCONNECT_H
