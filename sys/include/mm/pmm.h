#ifndef _MM_PMM_H_
#define _MM_PMM_H_

#include <types.h>

void pmm_init(void);
void pmm_free(uintptr_t ptr, size_t frames);
uintptr_t pmm_alloc(size_t frames);

#endif
