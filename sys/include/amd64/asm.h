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
 *  Loads a value into CR8.
 */

void __amd64_load_cr8(size_t value);

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


typedef struct {
  int64_t rdi;
  int64_t rsi;
  int64_t rbx;
  int64_t rdx;
  int64_t rcx;
  int64_t rax;
  int64_t r8;
  int64_t r9;
  int64_t r10;
  int64_t r11;
  int64_t r12;
  int64_t r13;
  int64_t r14;
  int64_t r15;

  uint64_t fs;
  uint64_t gs;

  /* Other information */
  uint64_t trapno;
  uint64_t rip;
  uint16_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint16_t ss;
} trapframe_t;

#endif
