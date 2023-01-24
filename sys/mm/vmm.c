#include <mm/vmm.h>
#include <limine.h>

volatile struct limine_hhdm_request g_hhdm_request = {
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0
};
