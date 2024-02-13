/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Hello",
    /* First member's full name */
    "zhangchi",
    /* First member's email address */
    "2216336843@qq.com",
    /* Second member's full name */
    "zhangchi",
    /* Second member's email address */
    "2216336843@qq.com",
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)
#define MINBLOCKSIZE 16

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-DSIZE))

#define GET_PREV(bp) (*(unsigned int *)(bp))
#define SET_PREV(bp, prev) (*(unsigned int *)(bp) = (prev))

#define GET_NEXT(bp) (*((unsigned int *)(bp) + 1))
#define SET_NEXT(bp, next) (*((unsigned int *)(bp) + 1) = (next))

static char *heap_listp;
static char *free_list_head;

static void *coalesce(void *bp);
static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void insert_free_list();
static void remove_free_list(void *bp);
static int count = 0;

static void remove_free_list(void *bp) {
  if (bp == NULL || GET_ALLOC(HDRP(bp)))
    return;

  void *prev = GET_PREV(bp);
  void *next = GET_NEXT(bp);

  SET_PREV(bp, 0);
  SET_NEXT(bp, 0);

  if (prev == NULL && next == NULL)
    free_list_head = NULL;

  else if (prev == NULL && next != NULL) {
    SET_PREV(next, NULL);
    free_list_head = next;
  }

  else if (prev != NULL && next == NULL) {
    SET_NEXT(prev, NULL);
  }

  else if (prev != NULL && next != NULL) {
    SET_NEXT(prev, next);
    SET_PREV(next, prev);
  }
}

static void insert_free_list(void *bp) {
  if (bp == NULL)
    return;

  if (free_list_head == NULL) {
    free_list_head = bp;
    return;
  }

  SET_NEXT(bp, free_list_head);
  SET_PREV(free_list_head, bp);

  free_list_head = bp;
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
  if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
    return -1;
  PUT(heap_listp, 0);
  PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
  PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
  PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
  heap_listp += (2 * WSIZE);

  free_list_head = NULL;

  if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    return -1;

  return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
  // printf("count: %d\n", count++);

  size_t asize;
  size_t extendsize;
  char *bp;

  if (size == 0)
    return NULL;

  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }

  extendsize = MAX(asize, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    return NULL;
  place(bp, asize);
  return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
  if (ptr == NULL)
    return;

  size_t size = GET_SIZE(HDRP(ptr));

  PUT(HDRP(ptr), PACK(size, 0));
  PUT(FTRP(ptr), PACK(size, 0));

  SET_PREV(ptr, 0);
  SET_NEXT(ptr, 0);

  coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
  if (ptr == NULL)
    return mm_malloc(size);
  if (size == 0)
    mm_free(ptr);
  if (!ptr)
    return NULL;

  void *new_ptr = mm_malloc(size);
  if (new_ptr == NULL)
    return NULL;

  size_t copy_size = GET_SIZE(HDRP(ptr));
  if (size < copy_size)
    copy_size = size;

  memcpy(new_ptr, ptr, copy_size - WSIZE);
  mm_free(ptr);

  return new_ptr;
}

static void *coalesce(void *bp) {
  void *prev_bp = PREV_BLKP(bp);
  void *next_bp = NEXT_BLKP(bp);

  size_t prev_alloc = GET_ALLOC(FTRP(prev_bp));
  size_t next_alloc = GET_ALLOC(HDRP(next_bp));

  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc)
    ;

  else if (prev_alloc && !next_alloc) {
    remove_free_list(next_bp);
    size += GET_SIZE(HDRP(next_bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0)); // TODO: why not use next block?
  }

  else if (!prev_alloc && next_alloc) {
    remove_free_list(prev_bp);
    size += GET_SIZE(HDRP(prev_bp));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(prev_bp), PACK(size, 0));

    bp = prev_bp;
  }

  else {
    remove_free_list(prev_bp);
    remove_free_list(next_bp);

    size += GET_SIZE(HDRP(prev_bp)) + GET_SIZE(FTRP(next_bp));

    PUT(HDRP(prev_bp), PACK(size, 0));
    PUT(FTRP(next_bp), PACK(size, 0));

    bp = prev_bp;
  }

  insert_free_list(bp);

  return bp;
}

static void *extend_heap(size_t words) {
  char *bp;
  size_t size;

  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  if ((long)(bp = mem_sbrk(size)) == -1)
    return NULL;

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));

  SET_PREV(bp, 0);
  SET_NEXT(bp, 0);

  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

  return coalesce(bp);
}

static void *find_fit(size_t asize) {
  void *bp;

  for (bp = free_list_head; bp != NULL; bp = GET_NEXT(bp)) {
    if (GET_SIZE(HDRP(bp)) >= asize)
      return bp;
  }

  return NULL;
}

static void place(void *bp, size_t asize) {
  size_t csize = GET_SIZE(HDRP(bp));
  remove_free_list(bp);

  if ((csize - asize) >= MINBLOCKSIZE) {
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));

    bp = NEXT_BLKP(bp);

    PUT(HDRP(bp), PACK(csize - asize, 0));
    PUT(FTRP(bp), PACK(csize - asize, 0));

    SET_PREV(bp, 0);
    SET_NEXT(bp, 0);
    coalesce(bp);
  } else {
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}
