/*
 *  Description: Qnixx exception handlers.
 *  Author(s): Ian Marco Moffett.
 *
 */

#include <amd64/exceptions.h>
#include <amd64/interrupts.h>
#include <tty/console.h>

__dead static void
abort(void)
{
  __asm("cli; hlt");
  __builtin_unreachable();
}

/*
 *  Interrupt vector 0x0.
 */

__isr static void
divide_error(void* sf)
{
  printk(KERN_PANIC "Divide error.\n");
  abort();
}


/*
 *  Interrupt vector 0x1.
 */

__isr static void
debug_exception(void* sf)
{
  printk(KERN_DEBUG "DEBUG EXCEPTION\n");
}

/*
 *  Interrupt vector 0x3.
 */

__isr static void
breakpoint(void* sf)
{
  /* TODO: Sleep for 1000 ms */
}


/*
 *  Interrupt vector 0x4.
 */

__isr static void
overflow(void* sf)
{
  /* Do nothing */
}

/*
 * Interrupt vector 0x5.
 */

__isr static void
bound_range_exceeded(void* sf)
{
  printk(KERN_PANIC "Bound range exceeded.\n");
  abort();
}

/*
 * Interrupt vector 0x6.
 */

__isr static void
invalid_opcode(void* sf)
{
  printk(KERN_PANIC "Invalid opcode.\n");
  abort();
}

/*
 *  Interrupt vector 0x7.
 */

__isr static void
device_not_available(void* sf)
{
  printk(KERN_PANIC "Device not available (no math coprocessor).\n");
  abort();
}

/*
 *  Interrupt vector 0x8.
 */

__isr static void
double_fault(void* sf)
{
  printk(KERN_PANIC "Double fault.\n");
  abort();
}

/*
 *  Interrupt vector 0xA.
 */

__isr static void
invalid_tss(void* sf)
{
  printk(KERN_PANIC "Invalid TSS.\n");
  abort();
}

/*
 *  Interrupt vector: 0xB.
 */

__isr static void
segment_not_present(void* sf)
{
  printk(KERN_PANIC "Segment not present\n");
  abort();
}

/*
 *  Interrupt vector 0xC.
 */

__isr static void
stack_segment_fault(void* sf)
{
  printk(KERN_PANIC "Stack segment fault.\n");
  abort();
}

/*
 *  Interrupt vector: 0xD.
 */

__isr static void
general_protection_fault(void* sf)
{
  printk(KERN_PANIC "General protection fault.\n");
  abort();
}

/*
 *  Interrupt vector: 0xE.
 */

__isr static void
page_fault(void* sf)
{
  printk(KERN_PANIC "Page fault.\n");
  abort();
}

void 
init_exceptions(void)
{
  register_exception(0x0, divide_error);
  register_exception(0x1, debug_exception);
  register_exception(0x3, breakpoint);
  register_exception(0x4, overflow);
  register_exception(0x5, bound_range_exceeded);
  register_exception(0x6, invalid_opcode);
  register_exception(0x7, device_not_available);
  register_exception(0x8, double_fault);
  register_exception(0xA, invalid_tss);
  register_exception(0xB, segment_not_present);
  register_exception(0xC, stack_segment_fault);
  register_exception(0xD, general_protection_fault);
  register_exception(0xE, page_fault);
}
