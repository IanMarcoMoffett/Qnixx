#include <sched/sched.h>
#include <sync/mutex.h>
#include <amd64/cpu.h>

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

__dead 
void sched_start(void)
{
  for (;;)
  {
    __asm("hlt");
  }
}
