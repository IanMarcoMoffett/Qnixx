/*
 *  Description: Trap handler.
 *  Author(s): Ian Marco Moffett.
 */

.globl trap_entry

trap_entry:
  push %gs
  push %fs
  push %r15
  push %r14
  push %r13
  push %r12
  push %r11
  push %r10
  push %r9
  push %r8
  push %rax
  push %rcx
  push %rdx
  push %rbx
  push %rsi
  push %rdi

  mov %rsp, %rdi
  call trap
trap_end: 
  pop %rdi
  pop %rsi
  pop %rbx
  pop %rdx
  pop %rcx
  pop %rax
  pop %r8
  pop %r9
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15
  pop %fs
  pop %gs
  add $8, %rsp
  iretq
