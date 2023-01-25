/*
 *  Description: Kernel init.
 *  Author(s): Ian Marco Moffett.
 */

#include <cdefs.h>
#include <amd64/idt.h>
#include <amd64/exceptions.h>
#include <dev/video/fb.h>
#include <tty/console.h>
#include <mm/pmm.h>

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
