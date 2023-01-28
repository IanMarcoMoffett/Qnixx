#ifndef _AMD64_CPU_H_
#define _AMD64_CPU_H_

#include <amd64/lapic.h>

void __amd64_for_each_core(void(*kroutine)(void* sf));

/*
 *  Attempts to enable SSE.
 *  Returns zero on success.
 */

int __amd64_enable_sse(void);


/*
 *  Marks the processor as
 *  currently handling an IRQ.
 */

void irq_enter(void);

/*
 *  Marks the processor
 *  as no longer handling an IRQ.
 */

void irq_exit(void);

/*
 *  Marks the processor as executing
 *  critical code.
 */

void critical_enter(void);

/*
 *  Marks the processor as
 *  no longer executing
 *  critical code.
 */

void critical_exit(void);

#endif
