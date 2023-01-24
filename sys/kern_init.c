/*
 *  Description: Kernel init.
 *  Author(s): Ian Marco Moffett.
 */

#include <cdefs.h>
#include <amd64/idt.h>
#include <amd64/exceptions.h>
#include <tty/console.h>
#include <mm/pmm.h>

/*
 *  Initialize CPU related stuff.
 */

static void
init_mm(void)
{
  pmm_init();
}

static void
cpu_init(void)
{
  load_idt();
  init_exceptions();
  init_mm();
}

__dead void
_start(void)
{
  printk(KERN_INFO "INIT: Booting..\n");
  cpu_init();

  for (;;)
  {
    __asm("hlt");
  }
}
