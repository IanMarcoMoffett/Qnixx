#ifndef _AMD64_LAPIC_H_
#define _AMD64_LAPIC_H_

#define LAPIC_NO_SHORTHAND        0x0
#define LAPIC_SELF                0x1
#define LAPIC_ALL_INCLUDING_SELF  0x2
#define LAPIC_ALL_EXCLUDING_SELF  0x3

#include <types.h>

void lapic_init(void);
void lapic_send_ipi(uint8_t lapic_id, uint8_t vector, uint8_t shorthand);
void lapic_send_eoi(void);
uint8_t lapic_read_id(void);
uint8_t lapic_get_goto_vector(void);
uint8_t lapic_get_timer_vector(void);
void lapic_calibrate_timer(void);
void lapic_timer_oneshot(size_t ms);

#endif
