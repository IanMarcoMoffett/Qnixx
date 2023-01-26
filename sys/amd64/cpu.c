#include <amd64/cpu.h>
#include <amd64/interrupts.h>
#include <sync/mutex.h>

static mutex_t goto_vec_lock = 0;

void __amd64_for_each_core(void(*kroutine)(void* sf))
{
  mutex_acquire(&goto_vec_lock);

  uint8_t goto_vector = lapic_get_goto_vector();
  register_interrupt(goto_vector, kroutine);
  lapic_send_ipi(0, goto_vector, LAPIC_ALL_INCLUDING_SELF); 

  mutex_release(&goto_vec_lock);
}
