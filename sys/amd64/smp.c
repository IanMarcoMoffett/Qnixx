#include <amd64/smp.h>
#include <amd64/idt.h>
#include <amd64/lapic.h>
#include <tty/console.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <sync/mutex.h>
#include <string.h>
#include <cdefs.h>
#include <limine.h>

static volatile struct limine_smp_request smp_req = {
  .id = LIMINE_SMP_REQUEST,
  .revision = 1
};

static struct limine_smp_response* smp_resp = NULL;
static mutex_t ap_lock = 0;
static mutex_t this_core_lock = 0;
static size_t core_idx = 0;

/* CPU core list */
cpu_core_t* g_corelist = NULL;

/*
 *  Application processor entry.
 *
 *  This is just where the
 *  APs start executing kernel
 *  code.
 */

__dead static void
ap_entry(struct limine_smp_info* info)
{
  VMM_LOAD_PML4(info->extra_argument);
  load_idt();
  
  /* Acquire the AP lock (protects core_idx) */
  mutex_acquire(&ap_lock);
  
  /* Load the GDT and TSS */
  load_gdt(g_corelist[core_idx].gdtr); 
  load_tss();
  
  /* Setup the local APIC */
  lapic_init();
  lapic_timer_calibrate();
  
  /* Release the AP lock */
  mutex_release(&ap_lock);

  for (;;)
  {
    __asm("cli; hlt");
  }
}

static void
init_core_desc(cpu_core_t* desc, uint8_t lapic_id)
{
  desc->lapic_id = lapic_id;
  desc->gdt = kmalloc(sizeof(gdt_desc_t) * 13);
  memcpy(desc->gdt, g_gdt, sizeof(g_gdt));
  
  desc->gdtr = kmalloc(sizeof(gdtr_t));
  desc->gdtr->limit = sizeof(g_gdt) - 1;
  desc->gdtr->ptr = (uint64_t)desc->gdt;
  desc->lock = 0;
  desc->flags = 0;
  desc->running_process = NULL;
  TAILQ_INIT(&desc->processq_head);

  desc->tss = kmalloc(sizeof(tss_entry_t));
  write_tss((tss_desc_t*)&desc->gdt[GDT_TSS],
            desc->tss);
}

cpu_core_t*
this_core(void)
{
  mutex_acquire(&this_core_lock);
  for (size_t i = 0; i < smp_resp->cpu_count; ++i)
  {
    if (g_corelist[i].lapic_id == lapic_read_id())
    {
      mutex_release(&this_core_lock);
      return &g_corelist[i];
    }
  }

  mutex_release(&this_core_lock);
  return NULL;
}


size_t
smp_get_core_count(void)
{
  return smp_resp->cpu_count;
}

void
smp_init(void)
{
  smp_resp = smp_req.response;
  g_corelist = kmalloc(sizeof(cpu_core_t) * smp_resp->cpu_count);
  printk(KERN_INFO "CPU has %d cores.\n", smp_resp->cpu_count);

  /* Set up each core in the corelist */
  for (size_t i = 0; i < smp_resp->cpu_count; ++i)
  {
    init_core_desc(&g_corelist[i], smp_resp->cpus[i]->lapic_id);
  }

  /* Bootstrap APs (application processors) */
  printk(KERN_INFO "Bootstrapping %d APs..\n", 
         smp_resp->cpu_count - 1);

  for (size_t i = 0; i < smp_resp->cpu_count; ++i)
  {
    if (smp_resp->cpus[i]->lapic_id == smp_resp->bsp_lapic_id)
    {
      continue;
    }
 
    smp_resp->cpus[i]->extra_argument = (uintptr_t)vmm_make_pml4();
    smp_resp->cpus[i]->goto_address = ap_entry;
  }
}
