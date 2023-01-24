#include <amd64/interrupts.h>
#include <amd64/idt.h>

void register_interrupt(uint8_t vector, void(*isr)(void* sf))
{
  idt_set_desc(vector, isr, IDT_INT_GATE_FLAGS);
}

void register_exception(uint8_t vector, void(*isr)(void* sf))
{
  idt_set_desc(vector, isr, IDT_TRAP_GATE_FLAGS);
}
