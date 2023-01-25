.globl __amd64_load_cr3
.globl __amd64_read_cr3
.globl __amd64_rdmsr
.globl __amd64_wrmsr

__amd64_load_cr3:
  mov %rdi, %cr3
  retq

__amd64_read_cr3:
  mov %cr3, %rax
  retq

__amd64_rdmsr:
  mov %edi, %ecx
  rdmsr
  shl $32, %rdx
  or %rax, %rdx
  mov %rdx, %rax
  retq

__amd64_wrmsr:
  mov %edi, %ecx
  mov %rsi, %rax
  mov %rsi, %rdx
  shl $32, %rdx
  wrmsr
  retq
