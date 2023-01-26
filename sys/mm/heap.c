/*
 *  Description: Heap module.
 *  Author(s): Ian Marco Moffett.
 */

#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/tlsf.h>
#include <string.h>

static tlsf_t tlsf_ctx;
static size_t bytes_allocated = 0;

void
heap_init(void)
{
  tlsf_ctx = tlsf_create_with_pool(vmm_alloc_pages(HEAP_SIZE_PAGES), HEAP_SIZE_PAGES*4096);
}


void*
kmalloc(size_t sz)
{
  bytes_allocated += sz;
  return tlsf_malloc(tlsf_ctx, sz);
}

void*
kcalloc(size_t nmemb, size_t size)
{
  void* mem = kmalloc(size*nmemb);
  memzero(mem, size*nmemb);
  return mem;
}

void* 
krealloc(void* ptr, size_t sz)
{
  return tlsf_realloc(tlsf_ctx, ptr, sz);
}

void
kfree(void* ptr)
{
  tlsf_free(tlsf_ctx, ptr);
}
