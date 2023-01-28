#ifndef _ACPI_H_
#define _ACPI_H_

#include <types.h>

void* acpi_query(const char* query);
void acpi_init(void);
uint16_t acpi_remap_irq(uint8_t irq);

extern void* g_ioapic_mmio_base;
extern void* g_lapic_mmio_base;

#endif
