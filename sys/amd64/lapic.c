/*
 *  Description: Local APIC interface module.
 *  Author(s): Ian Marco Moffett.
 */

#include <amd64/lapic.h>
#include <amd64/cpuid.h>
#include <amd64/asm.h>
#include <amd64/smp.h>
#include <amd64/interrupts.h>
#include <mm/vmm.h>
#include <tty/console.h>
#include <dev/timer/hpet.h>

#define is_on_bsp() (__amd64_rdmsr(IA32_APIC) & (1 << 8))

#define IA32_APIC         0x1B
#define IA32_X2APIC_BASE  0x800

#define LAPIC_ID          0x020
#define LAPIC_SVR         0x0F0
#define LAPIC_ICR0        0x300
#define LAPIC_ICR1        0x310
#define LAPIC_SELF_IPI    0x3F0
#define LAPIC_EOI         0x0B0
#define LAPIC_TIMER_LVT   0x320
#define LAPIC_TIMER_CNT   0x390
#define LAPIC_TIMER_INIT  0x380
#define LAPIC_TIMER_DIV   0x3E0
#define IPI_LEVEL_ASSERT  (1 << 14)

static uintptr_t xapic_base = 0;
static uint8_t x2apic_supported = 0;

static uint8_t goto_vector = 0;
static uint8_t timer_vector = 0;

static void
xapic_write(uint32_t reg, uint64_t value)
{
  if (x2apic_supported)
  {
    __amd64_wrmsr(IA32_X2APIC_BASE + (reg >> 4), value);
  }
  else
  {
    uintptr_t base = xapic_base + VMM_HIGHER_HALF;
    *((volatile uint32_t*)base + reg) = value;
  }
}

static uint64_t
xapic_read(uint32_t reg)
{
  if (x2apic_supported)
  {
    return __amd64_rdmsr(IA32_X2APIC_BASE + (reg >> 4));
  }
  else
  {
    uintptr_t base = xapic_base + VMM_HIGHER_HALF;
    return *((volatile uint32_t*)base + reg);
  }
}

static inline uint8_t
supports_x2apic(void)
{
  uint32_t unused, ecx;
  __cpuid(1, unused, unused, ecx, unused);
  return (ecx & (1 << 21)) >> 21;
}

static inline void
enable_apic(void)
{
  /* Read the IA32_APIC MSR */
  uint64_t apic_msr_val = __amd64_rdmsr(IA32_APIC);

  /* Enable x2APIC if supported (setting bit 10) */
  apic_msr_val |= (x2apic_supported << 10);
  
  /* Hardware enable the Local APIC */
  apic_msr_val |= (1 << 11);

  /* Write the changes */
  __amd64_wrmsr(IA32_APIC, apic_msr_val);

  /* Now we must software enable the Local APIC */
  xapic_write(LAPIC_SVR, xapic_read(LAPIC_SVR) | (1 << 8) | 0xFF);

  /* Write a message if we are on the BSP */
  if (is_on_bsp())
  {
    printk(KERN_INFO "BSP: Local APIC enabled.\n");
  }
}


void
lapic_send_ipi(uint8_t lapic_id, uint8_t vector, uint8_t shorthand)
{
  uint32_t icr_low = 0;
  if (shorthand == LAPIC_SELF && x2apic_supported)
  {
    /* Try to use x2APIC SELF MSR */
    xapic_write(LAPIC_SELF_IPI, vector);
    return;
  }
    
  /* Encode vector and some other values into icr_low */
  icr_low |= vector;
  icr_low |= IPI_LEVEL_ASSERT;
  icr_low |= (shorthand << 18);
  
  /* Send the IPI */
  if (x2apic_supported)
  {
    xapic_write(LAPIC_ICR0, ((uint64_t)lapic_id << 32) | icr_low);
  }
  else
  {
    xapic_write(LAPIC_ICR1, ((uint32_t)lapic_id << 24));
    xapic_write(LAPIC_ICR0, icr_low);
    while (xapic_read(LAPIC_ICR0) & (1 << 12));
  }
}

void
lapic_calibrate_timer(void)
{
  xapic_write(LAPIC_TIMER_DIV, 0x3);
  xapic_write(LAPIC_TIMER_LVT, 0xFF | (1 << 16));
  xapic_write(LAPIC_TIMER_INIT, (uint32_t)-1);
  hpet_sleep(10);

  this_core()->lapic_freq = (((uint32_t)-1) - xapic_read(LAPIC_TIMER_CNT)) / 10ULL;
  xapic_write(LAPIC_TIMER_INIT, 0);
  xapic_write(LAPIC_TIMER_LVT, (1 << 16));
}


void
lapic_timer_oneshot(size_t ms)
{
  /* Stop the timer */
  xapic_write(LAPIC_TIMER_INIT, 0);
  xapic_write(LAPIC_TIMER_LVT, (1 << 16));

  /* Setup regs */
  xapic_write(LAPIC_TIMER_LVT, (xapic_read(LAPIC_TIMER_LVT) & ~(3 << 17)));
  xapic_write(LAPIC_TIMER_LVT, (xapic_read(LAPIC_TIMER_LVT) & 0xFFFFFF00) | timer_vector);
  xapic_write(LAPIC_TIMER_DIV, 0x3);
  xapic_write(LAPIC_TIMER_INIT, this_core()->lapic_freq * ms);
  xapic_write(LAPIC_TIMER_LVT, xapic_read(LAPIC_TIMER_LVT) & ~(1 << 16));
}

uint8_t
lapic_read_id(void)
{
  uint8_t shift = x2apic_supported ? 0 : 24;
  return xapic_read(LAPIC_ID) >> shift;
}

void
lapic_send_eoi(void)
{
  xapic_write(LAPIC_EOI, 0);
}

uint8_t
lapic_get_timer_vector(void)
{
  return timer_vector;
}

uint8_t
lapic_get_goto_vector(void)
{
  return goto_vector;
}

void
lapic_init(void)
{
  x2apic_supported = supports_x2apic();

  if (x2apic_supported)
  {
    xapic_base = __amd64_rdmsr(IA32_APIC) & 0xFFFFF000;
  }
  else if (is_on_bsp())
  {
    printk(KERN_INFO "BSP: CPUID.(EAX=1):ECX.21 is set.\n");
    printk(KERN_INFO "BSP: CPU supports x2APIC mode.\n");
  }

  enable_apic();

  goto_vector = alloc_int_vector();
  timer_vector = alloc_int_vector();
  register_interrupt(timer_vector, __lapic_timer_isr);
}
