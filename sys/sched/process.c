#include <sched/process.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <lib/bitmap.h>
#include <lib/assert.h>
#include <string.h>

static bitmap_t pid_bitmap = NULL;
static size_t pid_bitmap_bytes = 1;

static void
init_pid_bitmap(void)
{
  pid_bitmap = kmalloc(sizeof(uint8_t));
}

/*
 *  Allocates a process ID.
 *
 */

static pid_t
alloc_pid(void)
{
  if (pid_bitmap == NULL)
  {
    init_pid_bitmap();
  }

  size_t i = 1;

  while (1)
  {
    if ((i / 8) > pid_bitmap_bytes)
    {
      ++pid_bitmap_bytes;
      pid_bitmap = krealloc(pid_bitmap, sizeof(uint8_t) * (pid_bitmap_bytes + 2));

      assert(pid_bitmap != NULL);
    }

    if (!(bitmap_test_bit(pid_bitmap, i)))
    {
      bitmap_set_bit(pid_bitmap, i);
      return i;
    }

    ++i;
  }

  __builtin_unreachable();
}

__unused static void
free_pid(pid_t pid)
{
  assert(pid > 0);
  bitmap_unset_bit(pid_bitmap, pid);
}

static process_t*
create_process(const char* name, uid_t user_id, gid_t group_id)
{
  /* Ensure the name doesn't go higher than NAME_MAX */
  assert(strlen(name) <= NAME_MAX);

  /* Create a new process */
  process_t* p = kmalloc(sizeof(process_t));
  assert(p != NULL);

  /* Set the head_thread */
  p->head_thread = kmalloc(sizeof(thread_t));
  thread_t* head_thread = p->head_thread;

  /* Allocate a stack */
  head_thread->stack_base = (uintptr_t)kmalloc(0x500);
  assert(head_thread->stack_base != 0);

  head_thread->next = NULL;
  head_thread->parent = p;

  p->tail_thread = head_thread;
  p->thread_count = 1;
  p->running_thread = head_thread;
  p->next = NULL;

  /* Set the IDs */
  p->usr.uid = user_id;
  p->usr.euid = user_id;
  p->usr.gid = group_id;
  p->pid = alloc_pid();
  p->vaddrsp = vmm_make_pml4();
  memcpy(p->name, name, strlen(name));
  return p;
}

process_t*
create_kernel_process(const char* name, void(*entry)(void))
{
  process_t* p = create_process(name, 0, 0);
  trapframe_t* tf = &p->head_thread->tf;

  tf->ss = 0x30;
  tf->cs = 0x28;
  tf->rflags = 0x202;
  tf->rip = (uintptr_t)entry;
  return p;
}
