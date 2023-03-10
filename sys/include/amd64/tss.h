#ifndef _AMD64_TSS_H_
#define _AMD64_TSS_H_

#include <types.h>
#include <cdefs.h>

typedef struct
{
    uint32_t reserved1;
    uint32_t rsp0Low;
    uint32_t rsp0High;
    uint32_t rsp1Low;
    uint32_t rsp1High;
    uint32_t rsp2Low;
    uint32_t rsp2High;
    uint64_t reserved2;
    uint32_t ist1Low;
    uint32_t ist1High;
    uint32_t ist2Low;
    uint32_t ist2High;
    uint32_t ist3Low;
    uint32_t ist3High;
    uint32_t ist4Low;
    uint32_t ist4High;
    uint32_t ist5Low;
    uint32_t ist5High;
    uint32_t ist6Low;
    uint32_t ist6High;
    uint32_t ist7Low;
    uint32_t ist7High;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t io_base;
} __packed tss_entry_t;

typedef struct
{
    uint16_t seglimit;
    uint16_t baseAddrLow;
    uint8_t baseAddrMiddle;
    uint8_t type : 4;
    uint8_t zero : 1;
    uint8_t dpl : 2;
    uint8_t p : 1;
    uint8_t segLimitUpper : 4;
    uint8_t avl : 1;
    uint8_t unused : 2;
    uint8_t g : 1;
    uint8_t baseAddrOtherMiddle;
    uint32_t baseAddrUpper;
    uint32_t reserved;
} __packed tss_desc_t;

/*
 *  Writes needed values 
 *  to the TSS descriptor.
 */
void write_tss(tss_desc_t* gdt_tss, tss_entry_t* tss);

/*
 *  Writes and loads the TSS
 *  for the bootstrap processor.
 */

void load_tss_bsp(void);

/*
 *  Loads TSS for any other processor.
 */

void load_tss(void);

#endif
