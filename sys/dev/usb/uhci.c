#include <dev/usb/uhci.h>
#include <dev/pci/pci.h>
#include <tty/console.h>
#include <amd64/io.h>
#include <mm/heap.h>
#include <string.h>

#define CLASS_ID    0x0C
#define SUBCLASS_ID 0x03
#define INTERFACE   0x00

/* USB I/O registers */
#define USBCMD          0x00
#define USB_STS         0x02
#define USB_INTR_ENABLE 0x04
#define FRNUM           0x06
#define FLBASEADD       0x08
#define LEGSUP          0xC0

#define MAX_QH          8
#define MAX_TD          32

#define TD_PTR_TERMINATE (1 << 0)
#define TD_PTR_QH        (1 << 1)

#define CMD_RS           (1 << 0)

static pci_device_t* dev = NULL;
static uhci_controller_t* hc = NULL;

static uhci_qh_t*
alloc_qh(uhci_controller_t* hc)
{
  uhci_qh_t* end = (uhci_qh_t*)((uintptr_t)hc->qh_pool + MAX_QH);

  for (uhci_qh_t* qh = hc->qh_pool; qh != end; ++qh)
  {
    if (!(qh->active))
    {
      qh->active = 1;
      return qh;
    }
  }

  return NULL;
}

void
uhci_init(void)
{
  dev = pci_find_any(CLASS_ID, SUBCLASS_ID, INTERFACE);

  if (dev == NULL)
  {
    return;
  }

  printk(KERN_INFO "Found UHCI controller on PCI bus %d, slot %d\n",
         dev->bus, dev->slot);

  /* Allocate the HC structure and enable bus mastering */
  hc = kmalloc(sizeof(uhci_controller_t));
  pci_enable_bus_mastering(dev);
  
  /* Get the I/O base */
  hc->iobase = dev->bars[4] & 0xFFFFFFFC;

  /* Allocate memory for the framelist and queue head pool */
  hc->frame_list = kmalloc(sizeof(uint32_t) * 1024);
  hc->qh_pool = kmalloc(sizeof(uhci_qh_t) * MAX_QH);
  hc->td_pool = kmalloc(sizeof(uhci_td_t) * MAX_TD);
  
  /* Zero the pools */
  memzero(hc->qh_pool, sizeof(uhci_qh_t) * MAX_QH);
  memzero(hc->td_pool, sizeof(uhci_td_t) * MAX_TD);

  /* Setup frame list */
  uhci_qh_t* qh = alloc_qh(hc);
  qh->head = TD_PTR_TERMINATE;
  qh->element = TD_PTR_TERMINATE;

  for (uint32_t i = 0; i < 1024; ++i)
  {
    hc->frame_list[i] = (uint32_t)(uintptr_t)qh | TD_PTR_QH;
  }

  /* Disable legacy support and interrupts */
  __amd64_outw(hc->iobase + LEGSUP, 0x8F00);
  __amd64_outw(hc->iobase + USB_INTR_ENABLE, 0);

  /* Setup frame releated stuff */
  __amd64_outw(hc->iobase + FRNUM, 0);
  __amd64_outl(hc->iobase + FLBASEADD, (uintptr_t)hc->frame_list << 12);

  /* Clear the status */
  __amd64_outw(hc->iobase + USB_STS, 0xFFFF);

  /* Enable the controller */
  __amd64_outw(hc->iobase + USBCMD, CMD_RS);
}
