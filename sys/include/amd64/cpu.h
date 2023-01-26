#ifndef _AMD64_CPU_H_
#define _AMD64_CPU_H_

#include <amd64/lapic.h>

void __amd64_for_each_core(void(*kroutine)(void* sf));

#endif
