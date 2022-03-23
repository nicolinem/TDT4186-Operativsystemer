#ifndef ____BBUFFER___H___
#define ____BBUFFER___H___
#include "bbuffer.h"
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
/*
 * Bounded Buffer implementation to manage int values that supports multiple
 * readers and writers.
 *
 * The bbuffer module uses the sem module API to synchronize concurrent access
 * of readers and writers to the bounded buffer.
 */

/* Opaque type of a Bounded Buffer.
 * ...you need to figure out the contents of struct BNDBUF yourself
 */

typedef struct BNDBUF_t BNDBUF_t;
// typedef struct circular_buffer_t circular_buffer_t;

struct BNDBUF_t
{
  int *buff;
  int start;
  int end;
  int size;
  int count;
}

/* Creates a new Bounded Buffer.
 *
 * This function creates a new bounded buffer and all the helper data
 * structures required by the buffer, including semaphores for
 * synchronization. If an error occurs during the initialization the
 * implementation shall free all resources already allocated by then.
 *
 * Parameters:
 *
 * size     The number of integers that can be stored in the bounded buffer.
 *
 * Returns:
 *
 * handle for the created bounded buffer, or NULL if an error occured.
 */

BNDBUF_t *
bb_init(unsigned int size)
{
  assert(size >= 0);
  BNDBUF_t cbuf = malloc(sizeof(bb_init));
  memset(cb, 0, sizeof(cbuff_t));
  cbuf->size = size;
  cbuf->buff = (int *)malloc(sizeof(int) * size);

  return cbuf;
}

/* Destroys a Bounded Buffer.
 *
 * All resources associated with the bounded buffer are released.
 *
 * Parameters:
 *
 * bb       Handle of the bounded buffer that shall be freed.
 */

void bb_del(BNDBUF_t *bb)
{
  free(bb->buff)
  free(bb)
}

/* Retrieve an element from the bounded buffer.
 *
 * This function removes an element from the bounded buffer. If the bounded
 * buffer is empty, the function blocks until an element is added to the
 * buffer.
 *
 * Parameters:
 *
 * bb         Handle of the bounded buffer.
 *
 * Returns:
 *
 * the int element
 */

int bb_get(BNDBUF_t *bb) {
    int element;

    mutex_lock(&MonitorLock);
        while(count == 0) {
            cond_wait(&UntilNotEmpty, &MonitorLock); //waits if the buffer is not empty
        }



        

}




/* Add an element to the bounded buffer.
 *
 * This function adds an element to the bounded buffer. If the bounded
 * buffer is full, the function blocks until an element is removed from
 * the buffer.
 *
 * Parameters:
 *
 * bb     Handle of the bounded buffer.
 * fd     Value that shall be added to the buffer.
 *
 * Returns:
 *
 * the int element
 */

void bb_add(BNDBUF_t *bb, int fd)
{
  int end = bb->end;
  if (bb->count && (end % bb->size) == bb->start)
  {
    printf("Overflow Elem[%d] %d lost\n", bb->start, bb->buff[bb->start]);
    bb->start = (bb->start + 1) % bb->size;
    bb->count--;
  }
  printf("Added Elem[%d] = %d\n", bb->end, fd);
  bb->buff[bb->end] = fd;
  bb->end = (bb->end + 1) % bb->size;
  bb->count++;
}
}

#endif