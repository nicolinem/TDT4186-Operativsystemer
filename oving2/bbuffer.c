#include "bbuffer.h"
#include "sem.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct BNDBUF
{
    unsigned int size;
    int *head;
    int *tail;
    SEM *full;
    SEM *empty;
    int *buffer;
} BNDBUF;

/* Creates a new Bounded Buffer. */
BNDBUF *bb_init(unsigned int size)
{
    /* Free all allocated resources if an error occurs */
    BNDBUF *buff = malloc(sizeof(struct BNDBUF));
    if (buff == NULL)
    {
        printf("Failed to allocate buffer\n");
        free(buff);
    }
    buff->buffer = (int *)malloc(sizeof(int) * size);
    buff->size = size;
    buff->head = buff->buffer;
    buff->tail = buff->buffer;
    buff->full = sem_init(0);
    buff->empty = sem_init(size);

    return buff;
}

/* Destroys a Bounded Buffer and releases all associated resources */
void bb_del(BNDBUF *bb)
{
    sem_del(bb->empty);
    sem_del(bb->full);
    free(bb->buffer);

    free(bb);
}

/* Retrieve an element from the bounded buffer. 
 * If the bounded buffer is empty, the function blocks until an element is added to the buffer. */
int bb_get(BNDBUF *bb)
{
    P(bb->full);
    V(bb->empty);
    int returnInt = *bb->tail;
    bb->tail++;
    if (bb->tail == bb->buffer + bb->size)
        bb->tail = bb->buffer;
    return returnInt;
}


/* Adds an element to the bounded buffer. 
 * If the bounded buffer is full, the function blocks until an element is removed from the buffer. */
void bb_add(BNDBUF *bb, int fd)
{
    V(bb->full);
    *bb->head = fd;
    bb->head++;
    if (bb->head == bb->buffer + bb->size)
        bb->head = bb->buffer;
    P(bb->empty);
}
