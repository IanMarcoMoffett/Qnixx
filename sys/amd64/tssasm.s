/*
 *  Description: TSS loading.
 *  Author(s): Ian Marco Moffett.
 */

.globl load_tss

load_tss:
  str %ax
  mov $0x48, %ax
  or $3, %ax
  ltr %ax
  retq
