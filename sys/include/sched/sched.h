#ifndef _SCHED_H_
#define _SCHED_H_

#include <sched/process.h>
#include <amd64/gdt.h>
#include <amd64/tss.h>
#include <sync/mutex.h>
#include <types.h>
#include <cdefs.h>

/*
 *  Set if processor is executing
 *  critical code.
 */

#define P_EXEC_CRITICAL   (1 << 0)

/*
 *  Set if the processor
 *  is executing an interrupt
 *  service routine for an IRQ.
 */

#define P_IRQ            (1 << 1)

/*
 *  A CPU core descriptor.
 */

typedef struct
{
  uint8_t lapic_id;
  uint16_t flags;
  mutex_t lock;

  /* Global descriptor table stuff */
  gdt_desc_t* gdt;
  gdtr_t* gdtr;

  /* Task state segment */
  tss_entry_t* tss;
} cpu_core_t;

__dead void sched_start(void);

#endif
