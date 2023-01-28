#include <sched/sched.h>
#include <sync/mutex.h>
#include <amd64/cpu.h>
#include <amd64/smp.h>
#include <amd64/lapic.h>
#include <tty/console.h>

#define SCHED_DEBUG 1

static size_t current_core_idx = 0;

/*
 *  Returns a core descriptor.
 *
 */

static cpu_core_t*
sched_core(void)
{
  cpu_core_t* core = &g_corelist[current_core_idx];
  current_core_idx = (current_core_idx + 1) % smp_get_core_count();
  return core;
}

/*
 *  Enqueues a process into a core.
 *
 *  @param p: Process to add.
 */

static void
enqueue_process(process_t* p)
{
  /* Schedule a core to put the process on */
  cpu_core_t* core = sched_core();

  mutex_acquire(&core->lock);
  if (core->head_process == NULL)
  {
    core->head_process = p;
    core->tail_process = p;
    core->running_process = p;
  }
  else
  {
    core->tail_process->next = p;
    core->tail_process = p;
  }

  mutex_release(&core->lock);
}

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

__unused static void
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
    printk("Sched: Yielding thread (pid=%d)\n", core->running_process->pid);
    printk("Sched: Halting..\n");
  }

  __asm("cli; hlt");
}

__dead 
void sched_start(void)
{
  process_t* p = create_kernel_process("idle", idle);
  enqueue_process(p);

  __asm("sti");
  sched_begin();

  for (;;)
  {
    __asm("hlt");
  }
}
