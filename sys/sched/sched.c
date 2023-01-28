#include <sched/sched.h>
#include <sync/mutex.h>
#include <amd64/cpu.h>
#include <amd64/smp.h>
#include <amd64/lapic.h>
#include <tty/console.h>

#define SCHED_DEBUG 1

/*
 *  Enqueues a thread, meaning
 *  it adds a thread to the 
 *  process.
 *
 *  Basically what this does is simple..
 *  - Set the tail_thread's next field to `thread`.
 *  - Increment thread count.
 *
 *  @param p: Target process.
 *  @param thread: Thread to add.
 */

static inline void
enqueue_thread(process_t* p, thread_t* thread)
{
  p->tail_thread->next = thread;
  ++p->thread_count;
}

/*
 *  Begins the scheduler.
 */

static inline void
sched_begin(void)
{
  lapic_timer_calibrate();
  lapic_timer_oneshot(SCHED_QUANTUM_BASE);
}

__dead static void
idle(void)
{
  for (;;)
  {
    __asm("pause");
  }
}

void
sched_yield(trapframe_t* tf)
{
  cpu_core_t* core = this_core();
  
  /* 
   * Don't switch contexts if
   * the processor is handling 
   * an IRQ or executing critical code.
   */
  if (core->flags & (P_EXEC_CRITICAL | P_IRQ))
  {
    return;
  }

  if (SCHED_DEBUG)
  {
    printk("Sched: Yielding thread (pid=%d)\n");
    printk("Sched: Halting..\n");
  }

  __asm("cli; hlt");
}

__dead 
void sched_start(void)
{
  process_t* p = create_kernel_process("idle", idle);
  enqueue_thread(p, p->head_thread);

  __asm("sti");
  sched_begin();

  for (;;)
  {
    __asm("hlt");
  }
}
