#include <amd64/idt.h>


static idt_gate_descriptor_t idt[256];
static idtr_t idtr = {
  .limit = sizeof(idt_gate_descriptor_t) * 256 - 1,
  .base  = (uintptr_t)&idt
};


void 
register_interrupt(uint8_t vector, void(*isr)(void* sf), uint8_t flags)
{
  idt_gate_descriptor_t* desc = &idt[vector];
  uintptr_t addr = (uintptr_t)isr;

  desc->isr_low16 = (addr & 0xFFFF);
  desc->isr_mid16 = (addr >> 16) & 0xFFFF;
  desc->isr_high32 = (addr >> 32);
  desc->cs = 0x28;
  desc->ist = 0;
  desc->zero = 0;
  desc->zero1 = 0;
  desc->reserved = 0;
  desc->dpl = 3;
  desc->p = 1;
  desc->attr = flags;
}

void 
load_idt(void)
{
  __asm("lidt %0" :: "m" (idtr));
}
