#ifndef _AMD64_IOAPIC_H_
#define _AMD64_IOAPIC_H_

#include <types.h>

void ioapic_init(void);
void ioapic_set_entry(uint8_t index, uint64_t data);
uint16_t remap_irq(uint8_t irq);

#endif
