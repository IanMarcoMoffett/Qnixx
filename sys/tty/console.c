/*
 *  Description: Qnixx console logic.
 *  Author(s): Ian Marco Moffett.
 */

#include <tty/console.h>
#include <limine.h>
#include <string.h>

static volatile struct limine_terminal_request term_req = {
  .id = LIMINE_TERMINAL_REQUEST,
  .revision = 0
};

static void
puts(const char* str)
{
  term_req.response->write(term_req.response->terminals[0],
                           str, strlen(str));
}

void 
printk(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  for (const char* ptr = fmt; *ptr; ++ptr)
  {
    if (*ptr == '%') 
    {
      ++ptr;
      switch (*ptr) 
      {
        case 'd':
        case 'u':
        case 'l':
          puts(dec2str(va_arg(ap, uint64_t)));
          break;
        case 'x':
        case 'p':
          puts(hex2str(va_arg(ap, uint64_t)));
          break;
        case 'X':
          puts(hex2str(va_arg(ap, uint64_t)) + 2);
          break;
        case 's':
          puts(va_arg(ap, char*));
          break;
        case 'c':
          puts((char[2]){va_arg(ap, int), 0});
          break;
      }
    } 
    else 
    {
      puts((char[2]){*ptr, 0});
    }
  }

  va_end(ap);
}
