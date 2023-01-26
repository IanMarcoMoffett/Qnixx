/*
 *  Description: RTL8139 driver.
 *  Author(s): Ian Marco Moffett.
 */

#include <dev/net/rtl8139.h>
#include <dev/pci/pci.h>
#include <amd64/interrupts.h>
#include <amd64/io.h>
#include <amd64/lapic.h>
#include <tty/console.h>
#include <lib/math.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <string.h>
#include <cdefs.h>

#define VENDOR_ID 0x10EC
#define DEVICE_ID 0x8139

#define PORT(reg) (iobase + reg)

// Packets
#define PACKET_SIZE_MAX 0x600
#define PACKET_SIZE_MIN 0x16

// Buffers
#define RX_BUFFER_SIZE 32768
#define TX_BUFFER_SIZE PACKET_SIZE_MAX
#define TX_BUFFER_COUNT 4

// Registers
#define REG_MAC       0x00
#define REG_MAR0      0x08
#define REG_MAR4      0x12
#define REG_TXSTATUS0 0x10
#define REG_TXADDR0   0x20
#define REG_RXBUF     0x30
#define REG_COMMAND   0x37
#define REG_CAPR      0x38
#define REG_IMR       0x3C
#define REG_ISR       0x3E
#define REG_TXCFG     0x40
#define REG_RXCFG     0x44
#define REG_MPC       0x4C
#define REG_CFG9346   0x50
#define REG_CONFIG1   0x52
#define REG_MSR       0x58
#define REG_BMCR      0x62
#define REG_ANLPAR    0x68

// Commands
#define CMD_RX_EMPTY  0x01
#define CMD_TX_ENABLE 0x04
#define CMD_RX_ENABLE 0x08
#define CMD_RESET     0x10

// Register config
#define CFG9346_NONE  0x00
#define CFG9346_EEM0  0x40
#define CFG9346_EEM1  0x80

// Basic mode control register
#define BMCR_SPEED          0x2000
#define BMCR_AUTO_NEGOTIATE 0x1000
#define BMCR_DUPLEX         0x0100

#define MSR_LINKB 0x02
#define MSR_SPEED_10 0x08
#define MSR_RX_FLOW_CONTROL_ENABLE 0x40

// Recieve configuration
#define RXCFG_AAP               0x01
#define RXCFG_APM               0x02
#define RXCFG_AM                0x04
#define RXCFG_AB                0x08
#define RXCFG_AR                0x10
#define RXCFG_WRAP_INHIBIT      0x80
#define RXCFG_MAX_DMA_16B       0x000
#define RXCFG_MAX_DMA_32B       0x100
#define RXCFG_MAX_DMA_64B       0x200
#define RXCFG_MAX_DMA_128B      0x300
#define RXCFG_MAX_DMA_256B      0x400
#define RXCFG_MAX_DMA_512B      0x500
#define RXCFG_MAX_DMA_1K        0x600
#define RXCFG_MAX_DMA_UNLIMITED 0x0700
#define RXCFG_RBLN_8K           0x0000
#define RXCFG_RBLN_16K          0x0800
#define RXCFG_RBLN_32K          0x1000
#define RXCFG_RBLN_64K          0x1800
#define RXCFG_FTH_NONE          0xE000

// Transmit configuration
#define TXCFG_TXRR_ZERO         0x00
#define TXCFG_MAX_DMA_16B       0x000
#define TXCFG_MAX_DMA_32B       0x100
#define TXCFG_MAX_DMA_64B       0x200
#define TXCFG_MAX_DMA_128B      0x300
#define TXCFG_MAX_DMA_256B      0x400
#define TXCFG_MAX_DMA_512B      0x500
#define TXCFG_MAX_DMA_1K        0x600
#define TXCFG_MAX_DMA_2K        0x700
#define TXCFG_IFG11             0x3000000

// Interrupts
#define INT_RXOK               0x01
#define INT_RXERR              0x02
#define INT_TXOK               0x04
#define INT_TXERR              0x08
#define INT_RX_BUFFER_OVERFLOW 0x10
#define INT_LINK_CHANGE        0x20
#define INT_RX_FIFO_OVERFLOW   0x40
#define INT_LENGTH_CHANGE      0x2000
#define INT_SYSTEM_ERROR       0x8000

// Transfer status
#define TX_STATUS_OWN           0x2000
#define TX_STATUS_THRESHOLD_MAX 0x3F0000

// Recieve status
#define RX_MULTICAST             0x8000
#define RX_PHYSICAL_MATCH        0x4000
#define RX_BROADCAST             0x2000
#define RX_INVALID_SYMBOL_ERROR  0x0020
#define RX_RUNT                  0x0010
#define RX_LONG                  0x0008
#define RX_CRC_ERROR             0x0004
#define RX_FRAME_ALIGNMENT_ERROR 0x0002
#define RX_OK                    0x0001

static pci_device_t* dev = NULL;
static uint32_t iobase = 0;
static uint8_t* rxbuf = NULL;
static uint8_t txbufs[TX_BUFFER_COUNT];
static size_t next_txbuf = 0;
static ssize_t rxbuf_offset = 0;
static void* packet_buf = NULL;
mac_address_t g_rtl8139_mac_addr;

static void
update_mac_addr(void)
{
  for (unsigned int i = 0; i < 6; ++i)
  {
    g_rtl8139_mac_addr[i] = __amd64_inb(PORT(REG_MAC) + i);
  }
}

static inline uint8_t
link_up(void)
{
  return ((__amd64_inb(iobase + REG_MSR) & MSR_LINKB) == 0);
}

static inline uint8_t
get_speed(void)
{
  uint16_t msr = __amd64_inw(iobase + REG_MSR);
  return msr & MSR_SPEED_10 ? 10 : 100;
}

static void
startup_nic(void)
{
  /* Preform a software reset */
  __amd64_outb(PORT(REG_COMMAND), CMD_RESET);
  while (__amd64_inb(PORT(REG_COMMAND)) & CMD_RESET);

  /* Put the NIC in config write enable */
  __amd64_outb(PORT(REG_CFG9346), CFG9346_EEM0 | CFG9346_EEM1);

  /* Enable multicast */
  __amd64_outl(PORT(REG_MAR0), 0xFFFFFFFF);
  __amd64_outl(PORT(REG_MAR4), 0xFFFFFFFF);
  printk(KERN_INFO "RTL8139: Multicast enabled.\n");

  /* Enable RX and TX */
  __amd64_outb(PORT(REG_COMMAND), CMD_RX_ENABLE | CMD_TX_ENABLE);

  /* Turn on the NIC */
  __amd64_outb(PORT(REG_CONFIG1), 0);

  /* Setup RX buffers */
  rxbuf = kmalloc(RX_BUFFER_SIZE);
  __amd64_outl(PORT(REG_RXBUF), vmm_get_phys((uintptr_t)rxbuf));
  
  /* Use 100mbit full duplex auto negoiation mode */
  __amd64_outl(PORT(REG_BMCR), BMCR_SPEED 
                              | BMCR_AUTO_NEGOTIATE 
                              | BMCR_DUPLEX);

  printk(KERN_INFO "RTL8139: Using 100mbit full "
                   "duplex auto negoiation mode\n");

  /* Enable control flow */
  __amd64_outb(PORT(REG_MSR), MSR_RX_FLOW_CONTROL_ENABLE);

  /* 
   * Accept rtl8139 MAC match, multicast, and broadcasted packets.
   * Also use max DMA transfer size and no FIFO threshold
   */
  __amd64_outl(PORT(REG_RXCFG), RXCFG_APM 
                                | RXCFG_AM 
                                | RXCFG_AB 
                                | RXCFG_WRAP_INHIBIT 
                                | RXCFG_MAX_DMA_UNLIMITED 
                                | RXCFG_RBLN_32K 
                                | RXCFG_FTH_NONE);

  printk(KERN_INFO "RTL8139: Accepted packets: Mac match, Multicast, " 
                   "Broadcasted\n");
  printk(KERN_INFO "RTL8139: Use max DMA transfer "
                   "size and no FIFO threshold\n");

  /* 
   * Set TX mode to use default retry count, 
   * max DMA burst size and interframe gap time.
   */
  __amd64_outl(iobase + REG_TXCFG, TXCFG_TXRR_ZERO 
                                   | TXCFG_MAX_DMA_1K 
                                   | TXCFG_IFG11);

  
  /* Setup TX buffers */
  for (uint32_t i = 0; i < TX_BUFFER_COUNT; ++i)
  {
    txbufs[i] = vmm_get_phys((uintptr_t)kmalloc(TX_BUFFER_SIZE));
  }

  /* Re-lock config registers to prevent mistakes */
  __amd64_outb(PORT(REG_CFG9346), CFG9346_NONE);

  /* Set IMR, ISR */
  __amd64_outw(PORT(REG_IMR), INT_RXOK 
                              | INT_RXERR 
                              | INT_TXOK 
                              | INT_TXERR 
                              | INT_RX_BUFFER_OVERFLOW 
                              | INT_LINK_CHANGE 
                              | INT_RX_FIFO_OVERFLOW 
                              | INT_LENGTH_CHANGE 
                              | INT_SYSTEM_ERROR);
}

/*
 *  The receive path is a ring buffer.
 *
 *  Data coming in is first
 *  stored in a receive FIFO in
 *  the chip which then gets
 *  moved to the receive buffer.
 *
 *  Status is the first word in the packet.
 *  Length is the third word in the packet.
 *
 *  -- Ian.
 */

static void
recieve(void)
{
  uint8_t* packet = rxbuf + rxbuf_offset;
  uint16_t status = *(uint16_t*)packet;
  uint16_t len    = *(uint16_t*)(packet + 2);
  
  uint8_t is_error = !(status & RX_OK)
                     || (status & (RX_INVALID_SYMBOL_ERROR
                                   | RX_CRC_ERROR
                                   | RX_FRAME_ALIGNMENT_ERROR));

  if (is_error || (len >= PACKET_SIZE_MAX) || (len < PACKET_SIZE_MIN))
  {
    return;
  }

  memcpy(packet_buf, (uint8_t*)(packet + 4), len - 4);
  rxbuf_offset = ((rxbuf_offset + len + 4 + 3) & ~3) % RX_BUFFER_SIZE;

  __amd64_outw(PORT(REG_CAPR), rxbuf_offset - 0x10);
  rxbuf_offset %= RX_BUFFER_SIZE;
}

__isr static void
isr(void* sf)
{
  for (;;)
  {
    uint16_t status = __amd64_inw(PORT(REG_ISR));
    __amd64_outw(PORT(REG_ISR), status);

    uint8_t stat = status & (INT_RXOK 
                             | INT_RXERR 
                             | INT_TXOK 
                             | INT_TXERR 
                             | INT_RX_BUFFER_OVERFLOW 
                             | INT_LINK_CHANGE 
                             | INT_RX_FIFO_OVERFLOW 
                             | INT_LENGTH_CHANGE 
                             | INT_SYSTEM_ERROR);
    if (stat == 0)
    {
      break;
    }

    if (status & INT_RXOK)
    {
      /* We got a packet */
      recieve();
    }
  }

  __amd64_outw(PORT(REG_ISR), 0x5);
  lapic_send_eoi();
}


void
rtl8139_send_packet(void* data, size_t size)
{
  if (iobase == 0)
  {
    return;
  }

  ssize_t hwbuf = -1;
  for (uint32_t i = 0; i < TX_BUFFER_COUNT; ++i)
  {
    size_t canidate = (next_txbuf + i) % 4;
    uint32_t status = __amd64_inl(PORT(REG_TXSTATUS0 + (canidate * 4)));

    if (status & TX_STATUS_OWN)
    {
      hwbuf = canidate;
      break;
    }
  }

  next_txbuf = (hwbuf + 1) % 4;
  if (size < 60)
  {
     size = 60;
  }

  /* Send the packet */
  uint64_t phys_addr = txbufs[hwbuf];
  uint64_t virt_addr = phys_addr + VMM_HIGHER_HALF; 
  memzero((void*)(ALIGN_DOWN(virt_addr, 0x1000)), TX_BUFFER_SIZE - size);
  memcpy((void*)(ALIGN_DOWN(virt_addr, 0x1000)), data, size);
  __amd64_outl(PORT(REG_TXSTATUS0 + (hwbuf * 4)), size);
}

void
rtl8139_init(void)
{
  dev = pci_find(VENDOR_ID, DEVICE_ID);

  if (dev == NULL)
  {
    return;
  }
  
  /* Found a device */
  printk(KERN_INFO "RTL8139: Found RTL8139 card on PCI bus %d, slot %d\n",
         dev->bus, dev->slot);

  /* Enable bus mastering so the card can do DMA */
  pci_enable_bus_mastering(dev);

  /* Fetch the I/O base */
  iobase = dev->bars[0] & 0xFFFFFFFC;

  /* Startup the NIC */
  startup_nic();

  if (link_up())
  {
    printk(KERN_INFO "RTL8139: Link up @%dmbps!\n", get_speed());
  }
  else
  {
    printk(KERN_INFO "RTL8139: Link down..\n");
  }

  /* Allocate a packet buffer */
  packet_buf = kmalloc(PACKET_SIZE_MAX);

  /* Get the MAC address */
  update_mac_addr();
  printk(KERN_INFO "RTL8139: MAC address: %X:%X:%X:%X:%X:%X\n", 
         g_rtl8139_mac_addr[0], 
         g_rtl8139_mac_addr[1], 
         g_rtl8139_mac_addr[2], 
         g_rtl8139_mac_addr[3], 
         g_rtl8139_mac_addr[4], 
         g_rtl8139_mac_addr[5]);

  register_irq(dev->irq_line, isr);
}
