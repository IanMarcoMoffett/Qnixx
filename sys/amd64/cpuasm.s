/*
 *  Description: SSE support.
 *  Author(s): Ian Marco Moffett.
 */

.globl __amd64_enable_sse


.section .text
__amd64_enable_sse:
  mov $0x1, %eax
  cpuid
  test $1<<25, %edx
  jz .no_sse

  /* 
   * Alright, now to enable SSE!
   * - Clear CR0.EM (bit 2)
   * - Set CR0.MP (bit 1)
   * - Set CR4 OSFXSR (bit 9)
   * - Set CR4 OSXMMEXCPT (bit 10)
   */ 

  mov %cr0, %rax
  mov $1, %rbx
  shl $2, %rbx
  not %rbx
  and %rbx, %rax
  mov $1, %rbx
  or %rbx, %rax
  mov %rax, %cr0

  mov %cr4, %rax

  mov $1, %rbx
  shr $9, %rbx
  or %rbx, %rax

  mov $1, %rbx
  shr $10, %rbx
  or %rbx, %rax

  mov %rax, %cr4

  mov $0, %rax
  retq

.no_sse:
  mov $1, %rax
  retq
