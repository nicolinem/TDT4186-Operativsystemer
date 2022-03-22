#include "sem.h"

#include <stdio.h>

#include <semaphore.h>

// sem_t sem;

// int ret;

// int value;

/*

* Semaphore implementation for the synchronization of POSIX threads.

*

* This module implements counting P/V semaphores suitable for the

* synchronization of POSIX threads. POSIX mutexes and condition variables

* shall be utilized to implement the semaphore operations.

*/

/* Opaque type of a semaphore.

* ...you need to figure out the contents of struct SEM yourself!

*/

//trenger vi denne?
typedef struct SEM SEM;

/* Creates a new semaphore.

*

* This function creates a new semaphore. If an error occurs during the

* initialization, the implementation shall free all resources already

* allocated so far.

*

* Parameters:

*

* initVal      the initial value of the semaphore

*

* Returns:

*

* handle for the created semaphore, or NULL if an error occured.

*/

SEM *sem_init(int initVal){
  SEM *sem = malloc(sizeof(SEM))
  if (sem == NULL) {
    return NULL;
  }

ret = sem_init(&sem, 0, initVal);

sem_init(&sem, 0, initVal);

}

/* Destroys a semaphore and frees all associated resources.

*

* Parameters:

*

* sem           handle of the semaphore to destroy

*

* Returns:
* 0 on success, negative value on error. In case of an error, not all resources may have been freed, but
nevertheless the semaphore handle must not be used any more.
*/

//If successful, pthread_mutex_destroy() shall return zero; otherwise, an error number shall be returned 

int sem_del(SEM *sem){
  pthread_mutex_destroy(&sem->lock);
  pthread_cond_destroy(&sem->cond);
  free(sem);

  //sem_destroy(&sem);
}



/* P (wait) operation.

*

* Attempts to decrement the semaphore value by 1. If the semaphore value

* is 0, the operation blocks until a V operation increments the value and

* the P operation succeeds.

*

* Parameters:

*

* sem           handle of the semaphore to decrement

*/

void P(SEM *sem){

ret = sem_wait(&sem);

};

/* V (signal) operation.

*

* Increments the semaphore value by 1 and notifies P operations that are

* blocked on the semaphore of the change.

*

* Parameters:

*

* sem           handle of the semaphore to increment

*/



void V(SEM *sem){
  pthread_mutex_lock(&sem->lock);
  //increments semaphore value
  sem->value++;
  pthread_mutex_unlock(&sem->lock);
  pthread_cond_signal(&sem->cond);

  //ret = sem_post(&sem);;

};

