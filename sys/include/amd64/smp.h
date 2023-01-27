#ifndef _AMD64_SMP_H_
#define _AMD64_SMP_H_

#include <types.h>
#include <sched/sched.h>

void smp_init(void);

extern cpu_core_t* g_corelist;

#endif
