#ifndef _AMD64_INTERRUPTS_H_
#define _AMD64_INTERRUPTS_H_

#include <types.h>
#include <cdefs.h>

void register_interrupt(uint8_t vector, void(*isr)(void* sf));
void register_irq(uint8_t irq, void(*isr)(void* sf));
void register_exception(uint8_t vector, void(*isr)(void* sf));
uint8_t alloc_int_vector(void);

/* Interrupt service routines */
__isr void __lapic_timer_isr(void* sf);

#endif
