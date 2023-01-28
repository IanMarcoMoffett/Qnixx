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
#include <lib/assert.h>
#include <acpi/acpi.h>

// Local APIC Registers
#define LAPIC_ID                        0x0020  // Local APIC ID
#define LAPIC_VER                       0x0030  // Local APIC Version
#define LAPIC_TPR                       0x0080  // Task Priority
#define LAPIC_APR                       0x0090  // Arbitration Priority
#define LAPIC_PPR                       0x00a0  // Processor Priority
#define LAPIC_EOI                       0x00b0  // EOI
#define LAPIC_RRD                       0x00c0  // Remote Read
#define LAPIC_LDR                       0x00d0  // Logical Destination
#define LAPIC_DFR                       0x00e0  // Destination Format
#define LAPIC_SVR                       0x00f0  // Spurious Interrupt Vector
#define LAPIC_ISR                       0x0100  // In-Service (8 registers)
#define LAPIC_TMR                       0x0180  // Trigger Mode (8 registers)
#define LAPIC_IRR                       0x0200  // Interrupt Request (8 registers)
#define LAPIC_ESR                       0x0280  // Error Status
#define LAPIC_ICRLO                     0x0300  // Interrupt Command
#define LAPIC_ICRHI                     0x0310  // Interrupt Command [63:32]
#define LAPIC_TIMER                     0x0320  // LVT Timer
#define LAPIC_THERMAL                   0x0330  // LVT Thermal Sensor
#define LAPIC_PERF                      0x0340  // LVT Performance Counter
#define LAPIC_LINT0                     0x0350  // LVT LINT0
#define LAPIC_LINT1                     0x0360  // LVT LINT1
#define LAPIC_ERROR                     0x0370  // LVT Error
#define LAPIC_TICR                      0x0380  // Initial Count (for Timer)
#define LAPIC_TCCR                      0x0390  // Current Count (for Timer)
#define LAPIC_TDCR                      0x03e0  // Divide Configuration (for Timer)
#define LAPIC_TIMER_PERIODIC            0x20000
// Delivery Mode
#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

// Destination Mode
#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

// Delivery Status
#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

// Level
#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

// Trigger Mode
#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

// Destination Shorthand
#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000

static uint8_t timer_vector = 0;
static uint8_t goto_vector = 0;

/*
 *  Writes to a local APIC register
 *  via MMIO.
 *
 *  @param reg: Register to write to.
 *  @param value: Value to write.
 *
 */

static void write_mmio(uint16_t reg, uint32_t value)
{  
  uint64_t base = (uint64_t)g_lapic_mmio_base + VMM_HIGHER_HALF;
  *(volatile uint32_t*)(base + reg) = value;
}

/*
 *  Reads from a local APIC register
 *  via MMIO.
 *
 *  @param reg: Register to read.
 *  @returns: Value in register.
 */

static uint32_t read_mmio(uint16_t reg)
{
  uint64_t base = (uint64_t)g_lapic_mmio_base + VMM_HIGHER_HALF;
  return *(volatile uint32_t*)(base + reg);
}

void lapic_send_eoi(void)
{
  write_mmio(LAPIC_EOI, 0);
}

uint8_t lapic_get_timer_vector(void)
{
  return timer_vector;
}

void lapic_timer_calibrate(void)
{
  /* Set up calibration, with divisor set to 16 */
  write_mmio(LAPIC_TDCR, 0x3);
  write_mmio(LAPIC_TIMER, 0xFF | (1 << 16));
  write_mmio(LAPIC_TICR, (uint32_t)-1);

  /* Sleep for 10 ms */
  hpet_sleep(10);

  /* Set the frequency and disable the timer again */
  this_core()->lapic_freq = (((uint32_t)-1) - read_mmio(LAPIC_TCCR)) / 10ULL;
  write_mmio(LAPIC_TICR, 0);
  write_mmio(LAPIC_TICR, 1 << 16);
}

void lapic_timer_oneshot(uint64_t ms)
{
  /* Stop the timer */
  write_mmio(LAPIC_TICR, 0);
  write_mmio(LAPIC_TIMER, (1 << 16));

  /* Setup the registers */
  write_mmio(LAPIC_TIMER, timer_vector);
  write_mmio(LAPIC_TDCR, 3);
  write_mmio(LAPIC_TICR, this_core()->lapic_freq * ms);
}

void
lapic_init(void)
{
  if (goto_vector == 0 && timer_vector == 0)
  {
    goto_vector = alloc_int_vector();
    timer_vector = alloc_int_vector();
    register_interrupt(timer_vector, __lapic_timer_isr);
  }

  /* Use logical ID of 1 */
  write_mmio(LAPIC_LDR, 0x01000000);

  /* Enable Local APIC */
  write_mmio(LAPIC_SVR, (1 << 8) | 0xFF);
}


uint32_t
lapic_read_id(void)
{
  return read_mmio(LAPIC_ID) >> 24;
}

uint8_t
lapic_get_goto_vector(void)
{
  return goto_vector;
}

void
lapic_send_ipi(uint8_t lapic_id, uint8_t vector, uint8_t shorthand)
{
}
