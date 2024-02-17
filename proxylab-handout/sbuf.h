/* $begin sbuf.h */
#ifndef __SBUF_H__
#define __SBUF_H__

#include <semaphore.h>
#include "csapp.h"

typedef struct {
  int *buf;
  int n;
  int front;
  int rear;
  sem_t mutex;
  sem_t slots;
  sem_t items;
} sbuf_t;

#define SBUFSIZE 10
#define NTHREADS 10

void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);
void sbuf_init(sbuf_t *sbuf, int n);

#endif /* __SBUF_H__ */
/* $end sbuf.h */
