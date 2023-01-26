/*
 *  Description: UHCI Host controller driver.
 *  Author(s): Ian Marco Moffett.
 */

#include <dev/usb/uhci.h>
#include <dev/pci/pci.h>
#include <dev/timer/hpet.h>
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
#define PORT1           0x10

/* Constants */
#define MAX_QH          8
#define MAX_TD          32

/* Flags */
#define TD_PTR_TERMINATE        (1 << 0)
#define TD_PTR_QH               (1 << 1)

#define CMD_RS                  (1 << 0)
#define PORT_CONNECTION         (1 << 0)
#define PORT_CONNECTION_CHANGE  (1 << 1)
#define PORT_ENABLE_CHANGE      (1 << 3)
#define PORT_RESET              (1 << 9)
#define PORT_ENABLE             (1 << 2)
#define PORT_LS                 (3 << 4)    /* Line Status */
#define PORT_RD                 (1 << 6)    /* Resume Detect */
#define PORT_LSDA               (1 << 8)    /* Low Speed Device Attached */
#define PORT_RWC                (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE)

#define USB_LOW_SPEED     (1 << 0)
#define USB_FULL_SPEED    (1 << 1)

#define MAX_PORTS 10

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

static inline void
port_writew(uint32_t port, uint16_t word)
{
  uint32_t status = __amd64_inw(port);
  status |= word;
  status &= ~(PORT_RWC);
  __amd64_outl(port, status);
}

static inline void
port_clearw(uint32_t port, uint16_t word)
{
  uint32_t status = __amd64_inw(port);
  status &= ~(PORT_RWC);
  status &= ~(word);
  status |= PORT_RWC & word;
  __amd64_outl(port, status);
}


/*
 *  Returns port count < MAX_PORTS.
 */

static size_t
get_port_count(uhci_controller_t* hc)
{
  /*
   *  The UHCI spec says devices must have 2
   *  ports and you can't find how many there
   *  are. However if you look through the spec
   *  you'll see that bit 7 of the port status and
   *  control register is always set to 1 so you can
   *  check that. You also need to ensure 
   *  the register does not
   *  return all 1s (is nonexistant).
   */
  
  size_t port_count = 0;
  for (uint32_t port = 0; port < MAX_PORTS; ++port)
  {
    uint32_t status = __amd64_inw(hc->iobase + (PORT1 + (port * 2)));
    if (!(status & (1 << 7)) || status == 0xFFFF)
    {
      break;
    }

    ++port_count;
  }

  return port_count;
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

  printk(KERN_INFO "System has %d ports.\n", get_port_count(hc));
}
