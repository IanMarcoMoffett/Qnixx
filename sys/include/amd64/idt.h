#ifndef _AMD64_IDT_H_
#define _AMD64_IDT_H_

#define IDT_TRAP_GATE_FLAGS 0x8F
#define IDT_INT_GATE_FLAGS  0x8E
#define IDT_INT_GATE_USER   0xEE

#include <cdefs.h>
#include <types.h>

typedef struct
{
  uint16_t isr_low16;
  uint16_t cs;
  uint8_t ist   : 2;
  uint8_t zero  : 4;
  uint8_t attr  : 4;
  uint8_t zero1 : 1;
  uint8_t dpl   : 2;
  uint8_t p     : 1;
  uint16_t isr_mid16;
  uint32_t isr_high32;
  uint32_t reserved;
} idt_gate_descriptor_t;

typedef struct
{
  uint16_t limit;       /* Size of the IDT */ 
  uint64_t base;        /* Base address of the IDT */
} __packed idtr_t;

void idt_set_desc(uint8_t vector, void(*isr)(void* sf), uint8_t flags);
void load_idt(void);

#endif
