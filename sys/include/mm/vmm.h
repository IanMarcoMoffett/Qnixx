#ifndef _MM_VMM_H_
#define _MM_VMM_H_

extern volatile struct limine_hhdm_request g_hhdm_request;


#define PTE_ADDR_MASK 0x000FFFFFFFFFF000
#define LOWERHALF_MASK 0xFFFFFFFF
#define PTE_PRESENT (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER (1ULL << 2)
#define PTE_NX (1ULL << 63)
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define PAGE_SIZE 4096

#define VMM_HIGHER_HALF (g_hhdm_request.response->offset)


#endif
