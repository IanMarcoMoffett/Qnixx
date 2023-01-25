.globl __amd64_flush_tlb_single

__amd64_flush_tlb_single:
  invlpg (%rdi)
  retq

__amd64_flush_tlb:
  mov %cr3, %rax
  mov %rax, %cr3
  retq
