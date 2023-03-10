/*
 *  Description: Kernel init.
 *  Author(s): Ian Marco Moffett.
 */

#include <cdefs.h>
#include <amd64/idt.h>
#include <amd64/exceptions.h>
#include <amd64/lapic.h>
#include <amd64/ioapic.h>
#include <amd64/cpu.h>
#include <amd64/smp.h>
#include <amd64/gdt.h>
#include <amd64/tss.h>
#include <dev/video/fb.h>
#include <dev/timer/hpet.h>
#include <tty/console.h>
#include <mm/pmm.h>
#include <mm/heap.h>
#include <acpi/acpi.h>
#include <dev/init.h>
#include <fs/vfs.h>
#include <sched/sched.h>

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
  load_gdt_bsp();

  if (__amd64_enable_sse() == 0)
  {
    printk(KERN_INFO "INIT: SSE enabled.\n");
  }
  else
  {
    printk(KERN_INFO "INIT: Could not enable SSE; not supported.\n");
  }

  init_mm();
  acpi_init();

  ioapic_init();
  lapic_init();
  
  heap_init();
  load_tss_bsp();

  init_net(); 
  hpet_init();

  vfs_init();
  smp_init();
}

__dead void
_start(void)
{
  fb_init();
  printk(KERN_INFO "INIT: Booting..\n");
  init();
  
  /* Start the scheduler */
  sched_start();

  for (;;)
  {
    __asm("hlt");
  }
}
