#ifndef _SCHED_H_
#define _SCHED_H_

#include <amd64/gdt.h>
#include <amd64/tss.h>
#include <types.h>

/*
 *  A CPU core descriptor.
 */

typedef struct
{
  uint8_t lapic_id;

  /* Global descriptor table stuff */
  gdt_desc_t* gdt;
  gdtr_t* gdtr;

  /* Task state segment */
  tss_entry_t* tss;
} cpu_core_t;


#endif
