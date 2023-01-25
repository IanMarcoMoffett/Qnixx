#ifndef _AMD64_IO_H_
#define _AMD64_IO_H_

#include <types.h>
#include <cdefs.h>


uint8_t __amd64_inb(uint16_t port);
void __amd64_outb(uint16_t port, uint8_t data);
void __amd64_outw(uint16_t port, uint16_t data);
uint16_t __amd64_inw(uint16_t port);
void __amd64_outl(uint16_t port, uint32_t data);
uint32_t __amd64_inl(uint16_t port);
void __amd64_io_wait();

#endif
