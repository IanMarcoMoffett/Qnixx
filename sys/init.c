#include <cdefs.h>
#include <amd64/idt.h>

/*
 *  Initialize CPU related stuff.
 */

static void
cpu_init(void)
{
  load_idt();
}

__dead void
_start(void)
{
  cpu_init();

  for (;;)
  {
    __asm("hlt");
  }
}
