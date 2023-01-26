#ifndef _TIMER_HPET_H_
#define _TIMER_HPET_H_

#include <types.h>

void hpet_init(void);
void hpet_sleep(uint64_t ms);

#endif
