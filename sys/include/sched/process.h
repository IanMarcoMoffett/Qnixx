#ifndef _SCHED_PROCESS_H_
#define _SCHED_PROCESS_H_

#include <sys/types.h>
#include <sys/queue.h>
#include <amd64/asm.h>
#include <credentials.h>
#include <limits.h>

/*
 *  Set if this is the first
 *  on a core.
 */

#define THREAD_STARTUP   (1 << 0)

struct Process;

/*
 *  @tf: Trapframe.
 *  @stack_base: Base of the stack.
 *  @flags: Thread flags.
 *  @parent: Parent process.
 *  @next: Pointer to next thread.
 */

typedef struct Thread
{
  trapframe_t tf;
  uintptr_t stack_base;
  uint16_t flags;
  struct Process* parent;
  TAILQ_ENTRY(Thread) threadq;
} thread_t;

/*
 *  There is always
 *  one thread per process
 *  thus thread_count's minimal
 *  value is 1.
 *
 *  @name: Name of the process.
 *  @pid: Process ID.
 *  @usr: Some user information.
 *  @tail_thread: End of thread queue.
 *  @head_thread: Start of thread queue.
 *  @running_thread: Currently running thread.
 *  @thread_count: Number of threads (1 or higher).
 *  @vaddrsp: PML4 paging structure virtual address.
 */

typedef struct Process
{
  char name[NAME_MAX];
  pid_t pid;
  user_t usr;
  
  TAILQ_ENTRY(Process) runq;
  TAILQ_HEAD(, Thread) threadq_head;
  thread_t* running_thread;

  size_t thread_count;
  uintptr_t* vaddrsp;
} process_t;

/*
 *  Attempts to create a kernel process.
 *  Returns NULL on failure.
 *  
 *  @param name: Name of the process.
 *  @param entry: Entrypoint of process.
 *  @param show: 1 if should be added to process list.
 */

process_t* create_kernel_process(const char* name, void(*entry)(void));

#endif
