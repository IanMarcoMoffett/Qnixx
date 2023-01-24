#ifndef _AMD64_INTERRUPTS_H_
#define _AMD64_INTERRUPTS_H_

#include <types.h>

void register_interrupt(uint8_t vector, void(*isr)(void* sf));
void register_exception(uint8_t vector, void(*isr)(void* sf));

#endif
