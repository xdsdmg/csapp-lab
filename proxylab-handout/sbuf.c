#include "sbuf.h"

void sbuf_init(sbuf_t *sbuf, int n) {
  sbuf->buf = Calloc(n, sizeof(int));
  sbuf->n = n;
  sbuf->front = sbuf->rear = 0;
  Sem_init(&sbuf->mutex, 0, 1);
  Sem_init(&sbuf->slots, 0, n);
  Sem_init(&sbuf->items, 0, 0);
}

void sbuf_insert(sbuf_t *sp, int item) {
  P(&sp->slots);
  P(&sp->mutex);
  sp->buf[(++sp->rear) % (sp->n)] = item;
  V(&sp->mutex);
  V(&sp->items);
}

int sbuf_remove(sbuf_t *sp) {
  int item;

  P(&sp->items);
  P(&sp->mutex);

  item = sp->buf[(++sp->front) % (sp->n)];

  V(&sp->mutex);
  V(&sp->slots);

  return item;
}
