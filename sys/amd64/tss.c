/*
 *  Description: TSS helpers.
 *  Author(s): Ian Marco Moffett.
 */

#include <amd64/tss.h>
#include <amd64/gdt.h>
#include <string.h>
#include <mm/heap.h>

static tss_entry_t* tss = NULL;

void
load_tss_bsp(void)
{
  tss = kmalloc(sizeof(tss_entry_t));
  write_tss(g_bsp_tss_desc, tss);

  load_tss();
}

void
write_tss(tss_desc_t* gdt_tss, tss_entry_t* tss)
{
  memzero(tss, sizeof(tss_entry_t));

  uint64_t tss_base = (uint64_t)tss;
  uintptr_t stack = (uintptr_t)kmalloc(0x500) + (0x500 - 1);

  tss->rsp0Low = stack & (uint32_t)stack;
  tss->rsp0High = stack = (uint32_t)(stack >> 32);

  gdt_tss->seglimit = sizeof(tss_entry_t);
  gdt_tss->g = 0;

  gdt_tss->baseAddrLow = tss_base & 0xFFFF;
  gdt_tss->baseAddrMiddle = (tss_base >> 16) & 0xFF;
  gdt_tss->baseAddrOtherMiddle = (tss_base >> 24) & 0xFF;
  gdt_tss->baseAddrUpper = tss_base >> 32;
  gdt_tss->avl = 0;                                           // 0 for TSS.
  gdt_tss->dpl = 0;
  gdt_tss->p = 1;
  gdt_tss->reserved = 0;
  gdt_tss->type = 0x9;
  gdt_tss->zero = 0;
}
