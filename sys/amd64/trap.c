#include <sched/sched.h>
#include <amd64/asm.h>
#include <amd64/lapic.h>
#include <sync/mutex.h>

static mutex_t lock;

void trap(trapframe_t* tf)
{
  mutex_acquire(&lock);

  if (tf->trapno == lapic_get_timer_vector())
  {
    sched_yield(tf);
  }

  lapic_send_eoi();
  mutex_release(&lock);
}
