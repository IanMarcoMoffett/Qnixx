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
  uint8_t msi_cap;
} pci_device_t;

/*
 *  Enables bus mastering for a device.
 */
void pci_enable_bus_mastering(pci_device_t* dev);

/*
 *  Finds a device with vendor ID and device_id pair.
 */

pci_device_t* pci_find(uint32_t vendor_id, uint32_t device_id);

pci_device_t* pci_find_any(uint8_t class_id, uint8_t subclass_id, 
                           int8_t interface);

#endif
