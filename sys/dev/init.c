#include <dev/init.h>
#include <dev/net/rtl8139.h>

void init_net(void)
{
  rtl8139_init();
}
