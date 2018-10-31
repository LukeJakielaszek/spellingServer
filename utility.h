#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define CAPACITY 12

typedef struct sbuf_monitor_t{
  int *buf;
  int size;
  int front;
  int rear;
  pthread_mutex_t mutex;
  pthread_cond_t empty_slot;
  pthread_cond_t full_slot;
} sbuf_monitor_t;

void sbuf_init(sbuf_monitor_t *sp, int size);
void sbuf_free(sbuf_monitor_t * sp);
void sbuf_insert(sbuf_monitor_t * sp, int fd);
int sbuf_remove(sbuf_monitor_t * sp);

#endif /** UTILITY_H */
