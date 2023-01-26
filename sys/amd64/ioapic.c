#include <amd64/ioapic.h>
#include <acpi/def.h>
#include <acpi/acpi.h>
#include <tty/console.h>
#include <lib/assert.h>
#include <mm/vmm.h>

#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

// Memory mapped register for I/O APIC access.
#define IOREGSEL                        0x00
#define IOWIN                           0x10

// Other I/O APIC registers.
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10

static acpi_madt_t* madt = NULL;
static void* ioapic_base = NULL;

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
  uintptr_t base = (uintptr_t)ioapic_base + VMM_HIGHER_HALF;
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
  uintptr_t base = (uintptr_t)ioapic_base + VMM_HIGHER_HALF;
  *(volatile uint32_t*)base = reg;
  *(volatile uint32_t*)(base + IOWIN) = value;
}

static void
parse_madt(void)
{
  uint8_t* cur = (uint8_t*)(madt + 1);
  uint8_t* end = (uint8_t*)madt + madt->header.length;
  while (cur < end)
  {
    apic_header_t* apic_header = (apic_header_t*)cur;
    if (apic_header->type == APIC_TYPE_IO_APIC)
    {
      io_apic_t* ioapic = (io_apic_t*)cur;
      ioapic_base = (void*)(uint64_t)ioapic->io_apic_addr;
      printk(KERN_INFO "IOAPIC: MMIO base: %x\n", ioapic_base);
    }

    cur += apic_header->length;
  }
}


uint16_t
remap_irq(uint8_t irq)
{
  uint8_t* cur = (uint8_t*)(madt + 1);
  uint8_t* end = (uint8_t*)madt + madt->header.length;

  while (cur < end) 
  {
    apic_header_t* apic_header = (apic_header_t*)cur;
    if (apic_header->type == APIC_TYPE_INTERRUPT_OVERRIDE) 
    {
      apic_interrupt_override_t* intr_override = (apic_interrupt_override_t*)cur;
      if (intr_override->source == irq) 
      {
        return intr_override->interrupt;
      }
    }

    cur += apic_header->length;
  }

  return irq;
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
  madt = acpi_query("APIC");
  assert(madt != NULL); 
  parse_madt();

  uint32_t max_redirection_entries = ((read_mmio(IOAPICVER) >> 16) & 0xFF) + 1;

  /* Mask all redirection entries */
  for (uint32_t i = 0; i < max_redirection_entries; ++i)
  {
    ioapic_set_entry(i, 1 << 16);
  }
}
