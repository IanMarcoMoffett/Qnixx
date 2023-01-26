#ifndef SYNC_MUTEX_H_
#define SYNC_MUTEX_H_

#include <types.h>

typedef _Atomic volatile uint8_t mutex_t;

void mutex_acquire(mutex_t* lock);
void mutex_release(mutex_t* lock);

#endif
