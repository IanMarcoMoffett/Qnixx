/*
 *  Description: Interrupt module.
 *  Author(s): Ian Marco Moffett.
 *
 */

#include <amd64/interrupts.h>
#include <amd64/idt.h>
#include <amd64/ioapic.h>

void 
register_interrupt(uint8_t vector, void(*isr)(void* sf))
{
  idt_set_desc(vector, isr, IDT_INT_GATE_FLAGS);
}

void 
register_exception(uint8_t vector, void(*isr)(void* sf))
{
  idt_set_desc(vector, isr, IDT_TRAP_GATE_FLAGS);
}

void
register_irq(uint8_t irq, void(*isr)(void* sf))
{
  register_interrupt(0x20 + irq, isr);
  ioapic_set_entry(remap_irq(irq), 0x20 + irq);
}

uint8_t
alloc_int_vector(void)
{
  static uint8_t vector = 0x81;
  if (vector >= 255)
  {
    return 0;
  }

  return vector++;
}
