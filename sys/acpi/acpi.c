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

static volatile struct limine_rsdp_request rsdp_req = {
  .id = LIMINE_RSDP_REQUEST,
  .revision = 0
};

static acpi_rsdt_t* rsdt = NULL;
static size_t rsdt_entry_count = 0;

static uint8_t do_checksum(const acpi_header_t* header) 
{
  uint8_t sum = 0;

  for (uint32_t i = 0; i < header->length; ++i) 
  {
    sum += ((char*)header)[i];
  }

  return sum % 0x100 == 0;
}

void* acpi_query(const char* query)
{ 
  for (size_t i = 0; i < rsdt_entry_count; ++i)
  {
    acpi_header_t* current = (acpi_header_t*)((uint64_t)rsdt->tables[i] + VMM_HIGHER_HALF);
    if (memcmp(current->signature, query, strlen(query)) == 0)
    {
      return (void*)current;
    }
  }

  return NULL;
}


void acpi_init(void)
{
  /* Fetch the RSDP */
  acpi_rsdp_t* rsdp = rsdp_req.response->address;
  
  /* Fetch the RSDT */
  rsdt = (acpi_rsdt_t*)((uint64_t)rsdp->rsdtaddr + VMM_HIGHER_HALF);
  rsdt_entry_count = (rsdt->header.length - sizeof(rsdt->header)) / 4;
}
