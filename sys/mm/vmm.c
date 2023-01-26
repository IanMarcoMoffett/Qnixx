/*
 *  Description: Qnixx virtual memory manager.
 *  Author(s): Ian Marco Moffett.
 */

#include <mm/vmm.h>
#include <mm/pmm.h>
#include <amd64/tlbflush.h>
#include <limine.h>

#define VIRT_TO_PT_INDEX(virt) ((virt >> 12) & 0x1FF)

volatile struct limine_hhdm_request g_hhdm_request = {
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0
};

static uintptr_t*
get_next_level(uintptr_t* top_level, size_t index, uint8_t alloc)
{
  if (top_level[index] & PTE_PRESENT)
  {
    uintptr_t phys = PTE_GET_ADDR(top_level[index]);
    return (uintptr_t*)(phys + VMM_HIGHER_HALF);
  }

  if (!(alloc))
  {
    return NULL;
  }

  uintptr_t next_level = pmm_alloc(1);
  top_level[index] = next_level
                     | PTE_PRESENT
                     | PTE_WRITABLE
                     | PTE_NX;
  return (uintptr_t*)(next_level + VMM_HIGHER_HALF);
}

static uintptr_t*
get_page_table(uintptr_t* pml4, uintptr_t virt, uint8_t alloc)
{
  size_t pml4_index = (virt >> 39) & 0x1FF;
  size_t pdpt_index = (virt >> 30) & 0x1FF;
  size_t pd_index   = (virt >> 21) & 0x1FF;

  uintptr_t* pdpt = get_next_level(pml4, pml4_index, alloc);
  if (pdpt == NULL)
  {
    return NULL;
  }

  uintptr_t* pd = get_next_level(pdpt, pdpt_index, alloc);
  if (pd == NULL)
  {
    return NULL;
  }

  uintptr_t* pt = get_next_level(pd, pd_index, alloc);
  return pt;
}

void
vmm_map_page(uintptr_t* pml4, uintptr_t virt, uintptr_t phys, size_t flags)
{
  uintptr_t* page_table = get_page_table(pml4, virt, 1);
  page_table[VIRT_TO_PT_INDEX(virt)] = phys | flags;
  __amd64_flush_tlb_single(virt);
}


void
vmm_unmap_page(uintptr_t* pml4, uintptr_t virt)
{
  uintptr_t* page_table = get_page_table(pml4, virt, 0);
  if (page_table == NULL)
  {
    return;
  }

  page_table[VIRT_TO_PT_INDEX(virt)] = 0;
  __amd64_flush_tlb_single(virt);
}

void* vmm_alloc_pages(size_t pages)
{
  uintptr_t ptr = pmm_alloc(pages);
  return (void*)(ptr + VMM_HIGHER_HALF);
}
