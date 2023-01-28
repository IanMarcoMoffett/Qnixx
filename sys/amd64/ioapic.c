#include <amd64/ioapic.h>
#include <acpi/acpi.h>
#include <tty/console.h>
#include <lib/assert.h>
#include <mm/vmm.h>

// Memory mapped register for I/O APIC access.
#define IOREGSEL                        0x00
#define IOWIN                           0x10

// Other I/O APIC registers.
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10


/*
 *  Reads from an I/O APIC
 *  register via MMIO.
 *
 *  @param reg: Register to read.
 *  @returns: Value in register.
 *
 */

static uint32_t read_mmio(uint8_t reg)
{
  uintptr_t base = (uintptr_t)g_ioapic_mmio_base + VMM_HIGHER_HALF;
  *(volatile uint32_t*)base = reg;
  return *(volatile uint32_t*)(base + IOWIN);
}


/*
 *  Writes a value to an
 *  I/O APIC register
 *  via MMIO.
 *
 *  @param reg: Register to write.
 *  @param value: Value to write.
 *
 */

static void write_mmio(uint8_t reg, uint32_t value)
{
  uintptr_t base = (uintptr_t)g_ioapic_mmio_base + VMM_HIGHER_HALF;
  *(volatile uint32_t*)base = reg;
  *(volatile uint32_t*)(base + IOWIN) = value;
}

void
ioapic_set_entry(uint8_t index, uint64_t data)
{
  write_mmio(IOREDTBL + index * 2, (uint32_t)data);
  write_mmio(IOREDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}

void
ioapic_init(void)
{
  uint32_t max_redirection_entries = ((read_mmio(IOAPICVER) >> 16) & 0xFF) + 1;

  /* Mask all redirection entries */
  for (uint32_t i = 0; i < max_redirection_entries; ++i)
  {
    ioapic_set_entry(i, 1 << 16);
  }
}
