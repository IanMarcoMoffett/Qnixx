#ifndef _AMD64_CPU_H_
#define _AMD64_CPU_H_

#include <amd64/lapic.h>

void __amd64_for_each_core(void(*kroutine)(void* sf));

/*
 *  Attempts to enable SSE.
 *  Returns zero on success.
 */

int __amd64_enable_sse(void);

#endif
