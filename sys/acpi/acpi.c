/*
 *  Description: ACPI interface module.
 *  Author(s): Ian Marco Moffett.
 */

#include <acpi/acpi.h>
#include <acpi/def.h>
#include <string.h>
#include <limine.h>
#include <mm/vmm.h>
#include <tty/console.h>
#include <lib/assert.h>

#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

static volatile struct limine_rsdp_request rsdp_req = {
  .id = LIMINE_RSDP_REQUEST,
  .revision = 0
};

static acpi_rsdt_t* rsdt = NULL;
static acpi_madt_t* madt = NULL;
static size_t rsdt_entry_count = 0;

void* g_ioapic_mmio_base = NULL;
void* g_lapic_mmio_base  = NULL;

static void
parse_madt(void)
{
  g_lapic_mmio_base = (void*)(uint64_t)(madt->lapic_addr);
  uint8_t* cur = (uint8_t*)(madt + 1);
  uint8_t* end = (uint8_t*)madt + madt->header.length;
  while (cur < end)
  {
    apic_header_t* apic_header = (apic_header_t*)cur;
    if (apic_header->type == APIC_TYPE_IO_APIC)
    {
      io_apic_t* ioapic = (io_apic_t*)cur;
      g_ioapic_mmio_base = (void*)(uint64_t)ioapic->io_apic_addr;
      printk(KERN_INFO "IOAPIC: MMIO base: %x\n", g_ioapic_mmio_base);
    }

    cur += apic_header->length;
  }
}

static uint8_t
do_checksum(const acpi_header_t* header) 
{
  uint8_t sum = 0;

  for (uint32_t i = 0; i < header->length; ++i) 
  {
    sum += ((char*)header)[i];
  }

  return sum % 0x100 == 0;
}


void*
acpi_query(const char* query)
{ 
  for (size_t i = 0; i < rsdt_entry_count; ++i)
  {
    acpi_header_t* current = (acpi_header_t*)((uint64_t)rsdt->tables[i] + VMM_HIGHER_HALF);
    if (memcmp(current->signature, query, strlen(query)) == 0)
    {
      return do_checksum(current) ? (void*)current : NULL;
    }
  }

  return NULL;
}

uint16_t
acpi_remap_irq(uint8_t irq)
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
acpi_init(void)
{
  /* Fetch the RSDP */
  acpi_rsdp_t* rsdp = rsdp_req.response->address;
  
  /* Fetch the RSDT */
  rsdt = (acpi_rsdt_t*)((uint64_t)rsdp->rsdtaddr + VMM_HIGHER_HALF);
  rsdt_entry_count = (rsdt->header.length - sizeof(rsdt->header)) / 4;

  madt = acpi_query("APIC");
  assert(madt != NULL); 
  parse_madt();
}
