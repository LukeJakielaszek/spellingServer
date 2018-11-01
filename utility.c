#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "dictionary.h"

#define CAPACITY 12

typedef struct sbuf_monitor_t{
  int *buf;
  int capacity;
  int count;
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

// remove fd from monitor's buffer
int sbuf_remove(sbuf_monitor_t * sp){
  // removed fd
  int fd;

  // locks monitor
  if(pthread_mutex_lock(&sp->mutex) != 0){
    printf("ERROR: Unable to lock mutex.\n");
    exit(EXIT_FAILURE);
  }

  // checks if any slots have an item
  while(sp->count == 0){
    if(pthread_cond_wait(&sp->full_slot, &sp->mutex)){
      printf("ERROR: Unable to wait on full slot.\n");
      exit(EXIT_FAILURE);
    }
  }

  // removes item
  fd = sp->buf[(++sp->front) % (sp->capacity)];

  sp->count--;

  // signals producers
  if(pthread_cond_signal(&sp->empty_slot) != 0){
      printf("ERROR: Unable to signal empty slot.\n");
      exit(EXIT_FAILURE);
  }

  // unlocks monitor
  if(pthread_mutex_unlock(&sp->mutex)!=0){
    printf("ERROR: Unable to unlock mutex.\n");
    exit(EXIT_FAILURE);
  }

  // returns file descriptor
  return fd;
}

// inserts file descriptor into monitor
void sbuf_insert(sbuf_monitor_t * sp, int fd){
  // lock monitor
  if(pthread_mutex_lock(&sp->mutex)!=0){
    printf("ERROR: Unable to lock mutex.\n");
    exit(EXIT_FAILURE);
  }

  // check if slot is available
  while(sp->count == sp->capacity){
    if(pthread_cond_wait(&sp->empty_slot, &sp->mutex) != 0){
      printf("ERROR: Unable to wait on empty slot.\n");
      exit(EXIT_FAILURE);
    }
  }

  // add fd to slot
  sp->buf[(++sp->rear) % (sp->capacity)] = fd;

  sp->count++;

  // signal consumers
  if(pthread_cond_signal(&sp->full_slot) != 0){
    printf("ERROR: Unable to signal full slot.\n");
    exit(EXIT_FAILURE);
  }

  // unlock monitor
  if(pthread_mutex_unlock(&sp->mutex)!=0){
    printf("ERROR: Unable to unlock mutex.\n");
    exit(EXIT_FAILURE);
  }
}

// initialize monitor
void sbuf_init(sbuf_monitor_t *sp, int size){
  // allocate buffer
  sp->buf = calloc(size, sizeof(int));

  // store max size
  sp->capacity = size;

  sp->count = 0;
  
  // position pointers
  sp->front = sp->rear = 0;

  // initialize mutex to binary
  pthread_mutex_init(&sp->mutex, NULL);

  // initialize conditionals
  pthread_cond_init(&sp->empty_slot, NULL);

  pthread_cond_init(&sp->full_slot, NULL);
}

// free monitor buffer
void sbuf_free(sbuf_monitor_t * sp){
  free(sp->buf);
}

