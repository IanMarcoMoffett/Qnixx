/*
 *  Description: Local APIC interface module.
 *  Author(s): Ian Marco Moffett.
 */

#include <amd64/lapic.h>
#include <amd64/cpuid.h>
#include <amd64/asm.h>
#include <mm/vmm.h>
#include <tty/console.h>

#define is_on_bsp() (__amd64_rdmsr(IA32_APIC) & (1 << 8))

#define IA32_APIC 0x1B
#define IA32_X2APIC_BASE 0x800
#define LAPIC_SVR 0x0F0

static uintptr_t xapic_base = 0;
static uint8_t x2apic_supported = 0;

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
lapic_init(void)
{
  x2apic_supported = supports_x2apic();

  if (!(supports_x2apic))
  {
    xapic_base = __amd64_rdmsr(IA32_APIC) & 0xFFFFF000;
  }
  else if (is_on_bsp())
  {
    printk(KERN_INFO "BSP: CPUID.(EAX=1):ECX.21 is set.\n");
    printk(KERN_INFO "BSP: CPU supports x2APIC mode.\n");
  }

  enable_apic();
}
