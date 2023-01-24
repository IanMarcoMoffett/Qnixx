#include <tty/console.h>
#include <limine.h>
#include <string.h>

static volatile struct limine_terminal_request term_req = {
  .id = LIMINE_TERMINAL_REQUEST,
  .revision = 0
};

void 
printk(const char* str)
{
  term_req.response->write(term_req.response->terminals[0],
                           str, strlen(str));
}
