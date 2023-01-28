#include <sched/sched.h>
#include <sync/mutex.h>
#include <amd64/cpu.h>

__dead 
void sched_start(void)
{
  for (;;)
  {
    __asm("hlt");
  }
}
