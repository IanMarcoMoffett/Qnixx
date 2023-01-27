#ifndef _MM_VMM_H_
#define _MM_VMM_H_

#include <types.h>
#include <limine.h>
#include <amd64/asm.h>

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
#define VMM_LOAD_PML4(pml4) \
  __amd64_load_cr3((uintptr_t)pml4 - VMM_HIGHER_HALF);

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

/*
 *  NOTE: Do not use too often
 *        as this is slow!!
 *  
 *  Returns physical address
 *  from virtual address passed.
 *
 *  @param virt: Virtual address.
 *
 */

uintptr_t vmm_get_phys(uintptr_t virt);

/*
 *  Returns current PML4.
 */ 

uintptr_t* vmm_get_pml4(void);

/*
 *  Makes PML4.
 */

uintptr_t* vmm_make_pml4(void);

#endif
