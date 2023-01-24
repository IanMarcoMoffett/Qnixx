#include <cdefs.h>
#include <amd64/idt.h>
#include <amd64/exceptions.h>
#include <tty/console.h>

/*
 *  Initialize CPU related stuff.
 */

static void
cpu_init(void)
{
  load_idt();
  init_exceptions();
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
