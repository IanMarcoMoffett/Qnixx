#ifndef _AMD64_ASM_H_
#define _AMD64_ASM_H_

#include <types.h>

/*
 *  Returns the value in CR3.
 */
uintptr_t __amd64_read_cr3(void);

/*
 *  Loads a value into CR3.
 */
void __amd64_load_cr3(uintptr_t val);

/*
 *  Reads an MSR.
 */

uint64_t __amd64_rdmsr(uint32_t msr);

/*
 *  Writes an MSR.
 */
uint64_t __amd64_wrmsr(uint32_t msr, uint64_t value);

#endif
