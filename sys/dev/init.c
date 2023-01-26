#include <dev/init.h>
#include <dev/net/rtl8139.h>
#include <dev/usb/uhci.h>

void init_net(void)
{
  rtl8139_init();
}


void init_usb(void)
{
  uhci_init();
}
