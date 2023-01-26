/*
 *  Description: Qnixx virtual memory manager.
 *  Author(s): Ian Marco Moffett.
 */

#include <mm/vmm.h>
#include <mm/pmm.h>
#include <amd64/tlbflush.h>
#include <amd64/asm.h>
#include <limine.h>
#include <string.h>
#include <lib/math.h>

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

void*
vmm_alloc_pages(size_t pages)
{
  uintptr_t ptr = pmm_alloc(pages);
  return (void*)(ptr + VMM_HIGHER_HALF);
}

uintptr_t*
vmm_get_pml4(void)
{
  uintptr_t cr3_val = __amd64_read_cr3();
  return (void*)(cr3_val + VMM_HIGHER_HALF);
}


uintptr_t*
vmm_make_pml4(void)
{
  static uintptr_t* old_pml4 = NULL;

  if (old_pml4 == NULL)
  {
    old_pml4 = vmm_get_pml4();
  }

  uintptr_t* new_pml4 = vmm_alloc_pages(1);
  memzero(new_pml4, 512);

  for (size_t i = 256; i < 512; ++i)
  {
    new_pml4[i] = old_pml4[i];
  }

  return new_pml4;
}

uintptr_t
vmm_get_phys(uintptr_t virt)
{
  virt = ALIGN_DOWN(virt, 0x1000);
  uintptr_t* page_table = get_page_table(vmm_get_pml4(), virt, 1);
  uintptr_t pte_value = page_table[VIRT_TO_PT_INDEX(virt)];
  return PTE_GET_ADDR(pte_value);
}
