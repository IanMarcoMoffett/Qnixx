#ifndef _NET_RTL8139_H_
#define _NET_RTL8139_H_

#include <types.h>
#include <net/ethernet.h>

void rtl8139_init(void);
void rtl8139_send_packet(void* data, size_t size);

#endif
