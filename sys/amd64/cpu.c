#include <amd64/cpu.h>
#include <amd64/smp.h>
#include <amd64/interrupts.h>
#include <sync/mutex.h>
#include <sched/sched.h>

static mutex_t goto_vec_lock = 0;

static void core_set_flags(uint16_t flags)
{
  cpu_core_t* core = this_core();

  mutex_acquire(&core->lock);
  core->flags |= flags;
  mutex_release(&core->lock);
}

static void core_clear_flags(uint16_t flags)
{
  cpu_core_t* core = this_core();

  mutex_acquire(&core->lock);
  core->flags &= ~(flags);
  mutex_release(&core->lock);
}

void __amd64_for_each_core(void(*kroutine)(void* sf))
{
  mutex_acquire(&goto_vec_lock);

  uint8_t goto_vector = lapic_get_goto_vector();
  register_interrupt(goto_vector, kroutine);
  lapic_send_ipi(0, goto_vector, LAPIC_ALL_INCLUDING_SELF); 

  mutex_release(&goto_vec_lock);
}

void
irq_enter(void)
{
  core_set_flags(P_IRQ);
}

void
irq_exit(void)
{
  core_clear_flags(P_IRQ);
}

void
critical_enter(void)
{
  core_set_flags(P_EXEC_CRITICAL);
}

void
critical_exit(void)
{
  core_clear_flags(P_EXEC_CRITICAL);
}
