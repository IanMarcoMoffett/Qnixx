#include <amd64/ioapic.h>
#include <acpi/def.h>
#include <acpi/acpi.h>
#include <tty/console.h>
#include <lib/assert.h>

#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

static acpi_madt_t* madt = NULL;
static void* ioapic_base = NULL;

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

void
ioapic_set_entry(uint8_t index, uint64_t data)
{
}

void
ioapic_init(void)
{
  madt = acpi_query("APIC");
  assert(madt != NULL); 
  parse_madt();
}
