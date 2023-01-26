#ifndef _MM_VMM_H_
#define _MM_VMM_H_

#include <types.h>
#include <limine.h>

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

/*
 *  Maps a virtual address to
 *  a physical address.
 *
 *  @param pml4: Pagemap level 4.
 *  @param virt: Virtual address.
 *  @param phys: Physical address.
 *  @param flags: PTE flags.
 */

void vmm_map_page(uintptr_t* pml4, uintptr_t virt, uintptr_t phys, 
                  size_t flags);

/*
 *  Unmaps a page.
 *
 *  @param pml4: Pagemap level 4.
 *  @param virt: Virtual address.
 */

void vmm_unmap_page(uintptr_t* pml4, uintptr_t virt);

/*
 *  Allocates n pages.
 *  
 *  @param pages: n.
 */

void* vmm_alloc_pages(size_t pages);

#endif
