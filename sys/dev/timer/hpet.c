#include <dev/timer/hpet.h>
#include <tty/console.h>
#include <acpi/acpi.h>
#include <acpi/def.h>
#include <cdefs.h>
#include <mm/vmm.h>
#include <sync/mutex.h>

#define HPET_REG_CAP 0x0
#define HPET_REG_CONF 0x10
#define HPET_REG_COUNTER 0x0F0

static acpi_hpet_t* hpet = NULL;
static void* hpet_base = NULL;
static uint64_t hpet_period = 0;
static mutex_t sleep_lock = 0;

static inline uint64_t hpet_read(uint16_t reg)
{
  return *((volatile uint64_t*)((uint64_t)hpet_base + reg));
}

static inline void hpet_write(uint16_t reg, uint64_t val)
{
  *((volatile uint64_t*)((uint64_t)hpet_base + reg)) = val;
}

void hpet_init(void)
{
  hpet = acpi_query("HPET");

  if (hpet == NULL)
  {
    printk(KERN_PANIC "Failed to query for HPET with ACPI..\n");
    __asm("hlt");
  }

  hpet_base = (void*)(hpet->base.base + VMM_HIGHER_HALF);

  /*
   *  Ensure everything is
   *  correct and not bogus.
   */

  uint32_t reg_count = ((hpet_read(HPET_REG_CAP) >> 8) & 0x1F) + 1;
  if (reg_count < 2 || ((hpet_read(HPET_REG_CAP) >> 32) == 0))
  {
    printk(KERN_PANIC "HPET does not support required capabilities!\n");
    __asm("hlt");
  }

  hpet_period = (hpet_read(HPET_REG_CAP) >> 32);
  hpet_write(HPET_REG_COUNTER, 0);
  hpet_write(HPET_REG_CONF, hpet_read(HPET_REG_CONF) | (1 << 0));

  printk(KERN_INFO "Queried \"HPET\" in ACPI tables.. done\n");
  printk(KERN_INFO "HPET_PERIOD=%x, HPET_REG_COUNT=%x\n", hpet_period, reg_count);
}

void hpet_sleep(uint64_t ms)
{
  mutex_acquire(&sleep_lock);
  uint64_t goal = hpet_read(HPET_REG_COUNTER) + (ms * (1000000000000 / hpet_period));

  while (hpet_read(HPET_REG_COUNTER) < goal)
  {
    __asm("pause");
  }
  mutex_release(&sleep_lock);
}
