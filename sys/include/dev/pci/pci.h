#ifndef _PCI_PCI_H_
#define _PCI_PCI_H_

#include <types.h>

typedef struct
{
  uint8_t irq_line;
  uintptr_t bars[6];
  uint8_t bus;
  uint8_t slot;
  uint8_t func;
} pci_device_t;

void pci_enable_bus_mastering(pci_device_t* dev);
pci_device_t* pci_find(uint32_t vendor_id, uint32_t device_id);

#endif
