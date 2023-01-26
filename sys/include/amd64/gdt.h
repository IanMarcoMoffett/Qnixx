#ifndef _AMD64_GDT_H_
#define _AMD64_GDT_H_

#include <cdefs.h>
#include <types.h>


typedef struct
{
  uint16_t limit;
  uint64_t ptr;
} __packed gdtr_t;

typedef struct
{
  uint16_t limit;
  uint16_t base_low;
  uint8_t  base_mid;
  uint8_t  access;
  uint8_t  granularity;
  uint8_t  base_hi;
} __packed gdt_desc_t;

extern gdt_desc_t g_gdt[13];

void load_gdt(gdtr_t* gdtr);
void load_gdt_bsp(void);

#endif
