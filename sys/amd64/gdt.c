#include <amd64/gdt.h>

gdt_desc_t g_gdt[13] = {
    {0},

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0x9A,
        .granularity = 0x00,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0x92,
        .granularity = 0x00,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0x9A,
        .granularity = 0xCF,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0x92,
        .granularity = 0xCF,
        .base_hi     = 0x00
    },

    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0x9A,
        .granularity = 0x20,
        .base_hi     = 0x00
    },

    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0x92,
        .granularity = 0x00,
        .base_hi     = 0x00
    },

    // User code: 0x7
    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0xFA,
        .granularity = 0xAF,
        .base_hi     = 0x00
    },

    // User data: 0x8
    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0xF2,
        .granularity = 0x00,
        .base_hi     = 0x00
    },

    // 0x9.
    {0}, {0}, {0}
};

static gdtr_t gdtr = {
  .limit = sizeof(g_gdt) - 1,
  .ptr = (uint64_t)&g_gdt
};


tss_desc_t* g_bsp_tss_desc = (tss_desc_t*)&g_gdt[0x9];

void
load_gdt(gdtr_t* gdtr)
{
  __asm("lgdt %0" :: "m" (*gdtr));
}

void
load_gdt_bsp(void)
{
  load_gdt(&gdtr);
}
