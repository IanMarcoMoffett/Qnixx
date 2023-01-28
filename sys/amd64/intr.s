/*
 *  Description: Interrupt service routines.
 *  Author(s): Ian Marco Moffett.
 */

.globl __lapic_timer_isr

.section .text
__lapic_timer_isr:
  call irq_enter      /* The scheduler will call irq_exit() */
  push %rbp
  push %rax
  push %rdi
  push %rsi
  push %rdx
  push %rcx
  push %rbx
  push %r8
  call lapic_get_timer_vector
  mov %al, timer_vector
  pop %r8
  pop %rbx
  pop %rcx
  pop %rdx
  pop %rsi
  pop %rdi
  pop %rax
  pop %rbp
  push (timer_vector)
  jmp trap_entry

.section .data
timer_vector: .byte 0
