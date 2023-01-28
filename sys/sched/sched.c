#include <sched/sched.h>
#include <sync/mutex.h>
#include <amd64/cpu.h>
#include <amd64/smp.h>
#include <amd64/lapic.h>
#include <tty/console.h>
#include <mm/vmm.h>
#include <string.h>

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
 *  Sets the running thread
 *  in a process control block
 *  to the next thread.
 */

static void
next_thread(process_t* p)
{
  if (p->running_thread->next != NULL)
  {
    p->running_thread = p->running_thread->next;
  }
  else
  {
    p->running_thread = p->head_thread;
  }
}

/*
 *  Sets the running process
 *  in the core descriptor
 *  to the next.
 */

static void
next_process(cpu_core_t* core)
{
  if (core->running_process->next != NULL)
  {
    core->running_process = core->running_process->next;
  }
  else
  {
    core->running_process = core->head_process;
  }
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
  process_t* current_process = core->running_process;
  thread_t* current_thread = current_process->running_thread;

  if (SCHED_DEBUG)
  {
    printk("Sched: Yielding thread (pid=%d)\n", core->running_process->pid);
    printk("Sched: Halting..\n");
  }
  
  /* 
   * Don't switch contexts if
   * the processor is handling 
   * an IRQ or executing critical code.
   */
  if (core->flags & (P_EXEC_CRITICAL | P_IRQ))
  {
    return;
  } 

  if (!(current_thread->flags & THREAD_STARTUP))
  {
    /*
     *  Store the current context
     *  in the Process Control Block (PCB),
     *
     *  switch to the next thread in the PCB
     *  so when we are back we can execute the next
     *  thread,
     *
     *  switch to the next process in the core descriptor.
     *
     *  NOTE: current_process and current_thread
     *        will not actually point to the current
     *        after this.
     */
    memcpy(&current_thread->tf, tf, sizeof(trapframe_t));
    next_thread(current_process);
    next_process(core);
  }
  else
  {
    current_thread->flags &= ~(THREAD_STARTUP);
  }

  /* Copy the saved context into the trapframe */
  memcpy(tf, &current_thread->tf, sizeof(trapframe_t));
  
  /* Switch address spaces and we are done */
  VMM_LOAD_PML4(core->running_process->vaddrsp);
  lapic_timer_oneshot(SCHED_QUANTUM_BASE);
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
