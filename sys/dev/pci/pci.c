#include <dev/pci/pci.h>
#include <amd64/io.h>

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


void pci_enable_bus_mastering(pci_device_t* dev)
{
  uint16_t val = config_readw(dev->bus, dev->slot, dev->func, 0x4);
  config_writew(dev->bus, dev->slot, dev->func, 0x4, (val | (1 << 2)
                                                          | (1 << 0)));
}
