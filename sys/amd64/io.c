#include <amd64/io.h>

uint8_t
__amd64_inb(uint16_t port)
{
    unsigned char result;
    __asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
 }


void
__amd64_outb(uint16_t port, uint8_t data)
{
    __asm("out %%al, %%dx" : :"a" (data), "d" (port));
}

void
__amd64_outw(uint16_t port, uint16_t data) {
    __asm("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

uint16_t
__amd64_inw(uint16_t port)
{
    uint16_t data;
    __asm("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}


void
__amd64_outl(uint16_t port, uint32_t data)
{
    __asm("outl %0, %w1" : : "a" (data), "Nd" (port));
}


uint32_t
__amd64_inl(uint16_t port)
{
    uint32_t data;
    __asm("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}


void
__amd64_io_wait()
{
    __asm("out %%al, %%dx" : :"a" (0x0), "d" (0x80));
}
