#ifndef _AMD64_ASM_H_
#define _AMD64_ASM_H_

#include <types.h>
#include <cdefs.h>
#include <amd64/cpu.h>

/*
 *  Attempts to halt every core on the system.
 */
#define __amd64_halt()  for (;;) { __amd64_for_each_core(__amd64_hlt_isr); }

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

/*
 *  Halts the current core.
 */
void __amd64_hlt(void);

/*
 *  An interrupt service routine
 *  for halting.
 */

void __amd64_hlt_isr(void* sf);

#endif
