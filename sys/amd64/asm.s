.globl __amd64_load_cr3
.globl __amd64_read_cr3

__amd64_load_cr3:
  mov %rdi, %cr3
  retq

__amd64_read_cr3:
  mov %cr3, %rax
  retq
