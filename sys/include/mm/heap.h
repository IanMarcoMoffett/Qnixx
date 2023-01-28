#ifndef HEAP_H_
#define HEAP_H_

#include <types.h>

#define HEAP_SIZE_PAGES 30


void heap_init(void);
void* kmalloc(size_t sz);
void* kcalloc(size_t nmemb, size_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t sz);

#endif
