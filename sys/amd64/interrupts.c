/*
 *  Description: Interrupt module.
 *  Author(s): Ian Marco Moffett.
 *
 */

#include <amd64/interrupts.h>
#include <amd64/idt.h>

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


uint8_t alloc_int_vector(void)
{
  static uint8_t vector = 0x81;
  if (vector >= 255)
  {
    return 0;
  }

  return vector++;
}
