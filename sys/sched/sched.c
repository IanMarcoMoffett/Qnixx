#include <sched/sched.h>
#include <sync/mutex.h>
#include <amd64/cpu.h>
#include <amd64/smp.h>
#include <amd64/lapic.h>
#include <tty/console.h>
#include <mm/vmm.h>
#include <string.h>
#include <lib/assert.h>

#define SCHED_DEBUG 1

static size_t current_core_idx = 0;
static mutex_t lock = 0;

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
 *  Appends a thread to
 *  the thread list.
 */

static void
enqueue_thread(process_t* parent, thread_t* thread)
{
  TAILQ_INSERT_TAIL(&parent->threadq_head, thread, threadq);
  ++parent->thread_count;
}

/*
 *  Appends a process to 
 *  the process list.
 */

static void
enqueue_process(process_t* p)
{
  cpu_core_t* core = sched_core();
  TAILQ_INSERT_TAIL(&core->processq_head, p, runq);

  if (core->running_process == NULL)
  {
    core->running_process = p;
  }
}

/*
 *  Goes to the next process
 *  and if there is no next
 *  wrap around to the first.
 */

static void
next_process(cpu_core_t* core)
{
  process_t* running = core->running_process;
  core->running_process = TAILQ_NEXT(running, runq);

  if (core->running_process == NULL)
  {
    core->running_process = TAILQ_FIRST(&core->processq_head);
  }
}

/*
 *  Same thing as next_process() but
 *  for threads.
 */

static void
next_thread(process_t* parent)
{
  thread_t* thread = parent->running_thread;
  parent->running_thread = TAILQ_NEXT(thread, threadq);

  if (parent->running_thread == NULL)
  {
    parent->running_thread = TAILQ_FIRST(&parent->threadq_head);
  }
}

/*
 *  Begins the scheduler.
 */

static inline void
sched_begin_timer(void)
{
  lapic_timer_calibrate();
  lapic_timer_oneshot(SCHED_QUANTUM_BASE);
}

__dead static void
idle(void)
{
  for (;;)
  {
    __asm("hlt");
  }
}

void
sched_yield(trapframe_t* tf)
{
  cpu_core_t* core = this_core();
  process_t* running = core->running_process;
  thread_t* thread = running->running_thread;

  if (!(thread->flags & THREAD_STARTUP))
  {
    memcpy(&thread->tf, tf, sizeof(trapframe_t));
    next_thread(running);
    next_process(core);

    running = core->running_process;
    thread = running->running_thread;
  }
  else
  {
    thread->flags &= ~(THREAD_STARTUP);
  }

  memcpy(tf, &thread->tf, sizeof(trapframe_t));
}

__dead 
void sched_start(void)
{
  process_t* p = create_kernel_process("idle", idle);
  p->running_thread->flags |= THREAD_STARTUP;
  enqueue_process(p);
  
  sched_begin_timer();
  __asm("sti");

  for (;;)
  {
    __asm("hlt");
  }
}
