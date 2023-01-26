/*
 *  Description: Kernel init.
 *  Author(s): Ian Marco Moffett.
 */

#include <cdefs.h>
#include <amd64/idt.h>
#include <amd64/exceptions.h>
#include <amd64/lapic.h>
#include <amd64/ioapic.h>
#include <dev/video/fb.h>
#include <tty/console.h>
#include <mm/pmm.h>
#include <acpi/acpi.h>

static void
init_mm(void)
{
  pmm_init();
}

static void
init(void)
{
  load_idt();
  init_exceptions();
  init_mm();
  acpi_init();
  ioapic_init();
  lapic_init();
}

__dead void
_start(void)
{
  fb_init();
  printk(KERN_INFO "INIT: Booting..\n");
  init();

  for (;;)
  {
    __asm("hlt");
  }
}
