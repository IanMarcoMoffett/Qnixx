#include <dev/pci/pci.h>
#include <amd64/io.h>
#include <lib/math.h>
#include <mm/heap.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

/*
 *  Read a word from the PCI configuration
 *  space.
 */

static uint16_t
config_readw(uint32_t bus, uint32_t slot, uint32_t func, uint8_t offset)
{
  uint32_t config_addr = (uint32_t)((1 << 31) 
                                    | (bus << 16)
                                    | (slot << 11)
                                    | (func << 8)
                                    | (offset & 0xFC));

  __amd64_outl(CONFIG_ADDRESS, config_addr);
  return (uint16_t)((__amd64_inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
}

/*
 *  Write a word to PCI configuration
 *  space.
 */

static void
config_writew(uint32_t bus, uint32_t slot, uint32_t func, 
              uint32_t offset, uint16_t value)
{
  uint32_t config_addr = (uint32_t)((1 << 31) 
                                    | (bus << 16)
                                    | (slot << 11)
                                    | (func << 8)
                                    | (offset & 0xFC));

  __amd64_outl(CONFIG_ADDRESS, config_addr);
  __amd64_outl(CONFIG_DATA, value);
}


static inline uint8_t
read_irq_line(uint8_t bus, uint8_t slot, uint8_t func)
{
  return config_readw(bus, slot, func, 0x3C) & 0xFF;
}

static inline uint16_t
read_vendor(uint8_t bus, uint8_t slot, uint8_t func)
{
  return config_readw(bus, slot, func, 0x0);
}

static inline uint16_t
read_device_id(uint8_t bus, uint8_t slot, uint8_t func)
{
  return config_readw(bus, slot, func, 0x2);
}

static inline uint8_t
read_class(uint8_t bus, uint8_t slot, uint8_t func)
{
  return config_readw(bus, slot, func, 0xA) >> 8;
}

static inline uint8_t
read_subclass(uint8_t bus, uint8_t slot, uint8_t func)
{
  return config_readw(bus, slot, func, 0xA) & 0xFF;
}


static inline uint32_t
get_bar0(uint8_t bus, uint8_t slot, uint8_t func) 
{
  uint16_t lo = config_readw(bus, slot, func, 0x10);
  uint16_t hi = config_readw(bus, slot, func, 0x12);
  return COMBINE_WORD(lo, hi);
}

static inline uint32_t
get_bar1(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint16_t lo = config_readw(bus, slot, func, 0x14);
  uint16_t hi = config_readw(bus, slot, func, 0x16);
  return COMBINE_WORD(lo, hi);
}

static inline uint32_t
get_bar2(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint16_t lo = config_readw(bus, slot, func, 0x18);
  uint16_t hi = config_readw(bus, slot, func, 0x1A);
  return COMBINE_WORD(lo, hi);
}

static inline uint32_t
get_bar3(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint16_t lo = config_readw(bus, slot, func, 0x1C);
  uint16_t hi = config_readw(bus, slot, func, 0x1E);
  return COMBINE_WORD(lo, hi);
}

static inline uint32_t
get_bar4(uint8_t bus, uint8_t slot, uint8_t func) 
{
  uint16_t lo = config_readw(bus, slot, func, 0x20);
  uint16_t hi = config_readw(bus, slot, func, 0x22);
  return COMBINE_WORD(lo, hi);
}

static inline uint32_t
get_bar5(uint8_t bus, uint8_t slot, uint8_t func) 
{
  uint16_t lo = config_readw(bus, slot, func, 0x24);
  uint16_t hi = config_readw(bus, slot, func, 0x26);
  return COMBINE_WORD(lo, hi);
}

static inline void
init_dev(pci_device_t* dev, uint8_t bus, uint8_t slot, uint8_t func)
{
  dev->irq_line = read_irq_line(bus, slot, func);
  dev->bars[0] = get_bar0(bus, slot, func);
  dev->bars[1] = get_bar1(bus, slot, func);
  dev->bars[2] = get_bar2(bus, slot, func);
  dev->bars[3] = get_bar3(bus, slot, func);
  dev->bars[4] = get_bar4(bus, slot, func);
  dev->bars[5] = get_bar5(bus, slot, func);
  dev->bus = bus;
  dev->slot = slot;
  dev->func = func;
}

void pci_enable_bus_mastering(pci_device_t* dev)
{
  uint16_t val = config_readw(dev->bus, dev->slot, dev->func, 0x4);
  config_writew(dev->bus, dev->slot, dev->func, 0x4, (val | (1 << 2)
                                                          | (1 << 0)));
}

pci_device_t*
pci_find(uint32_t vendor_id, uint32_t device_id)
{
  pci_device_t* dev = kmalloc(sizeof(pci_device_t));

  for (uint8_t bus = 0; bus < 5; ++bus)
  {
    for (uint8_t slot = 0; slot < 32; ++slot)
    {
      for (uint8_t func = 0; func < 8; ++func)
      {
        if (read_device_id(bus, slot, func) == device_id 
            && read_vendor(bus, slot, func) == vendor_id)
        {
          init_dev(dev, bus, slot, func);
          return dev;
        }
      }
    }
  }
  
  kfree(dev);
  return NULL;
}
