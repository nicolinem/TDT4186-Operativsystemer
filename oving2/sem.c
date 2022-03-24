#include "sem.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct SEM
{
  volatile int count;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} SEM;

/* Creates a new semaphore. */
SEM *sem_init(int initVal)
{
  SEM *semaphore = malloc(sizeof(struct SEM));
  semaphore->count = initVal;
  pthread_mutex_t mutex;

  /* Free semaphore and return NULL if an error occures. */
  if (pthread_mutex_init(&mutex, NULL) != 0)
  {
    free(semaphore);
    return NULL;
  }
  semaphore->mutex = mutex;

  pthread_cond_t cond;
  if (pthread_cond_init(&cond, NULL) != 0)
  {
    free(semaphore);
    return NULL;
  }

  semaphore->cond = cond;

  return semaphore;
}

/* Destroys a semaphore and frees all associated resources. */
int sem_del(SEM *sem)
{
  int errors = 0;
  /* Try to destroy mutex */
  if (pthread_mutex_destroy(&(sem->mutex)) != 0)
  {
    printf("Couldn't destroy mutex");
    return -1;
  }

  /* Try to destroy condition */
  if (pthread_cond_destroy(&(sem->cond)) != 0)
  {
    printf("Couldn't destroy cond");
    return -1;
  }
  free(sem);
  return 0;
}

/* P (wait) operation. Attempts to decrement the semaphore value by 1 */
void P(SEM *sem)
{
  pthread_mutex_lock(&(sem->mutex));
  while ((sem->count) == 0)
  {
    pthread_cond_wait(&(sem->cond), &(sem->mutex));
  }
  (sem->count)--;

  pthread_mutex_unlock(&(sem->mutex));
}

 /* V (signal) operation. Increments the semaphore and notifies P operations that are blocked on the semaphore of the change. */
void V(SEM *sem)
{
  pthread_mutex_lock(&(sem->mutex));

  if (sem->count == 0)
  {
    pthread_cond_signal(&(sem->cond));
  }
  (sem->count)++;
  pthread_mutex_unlock(&(sem->mutex));
}