/*
 *  Description: Qnixx console logic.
 *  Author(s): Ian Marco Moffett.
 */

#include <tty/console.h>
#include <tty/font.h>
#include <dev/video/fb.h>
#include <limine.h>
#include <string.h>

#define MAKE_FG_BG(fg, bg) ((uint64_t)fg << 32 | bg)
#define EXTRACT_FG(color) ((uint64_t)color >> 32)
#define EXTRACT_BG(color) (color & 0xFFFFFFFF)

static size_t _cy = 0, _cx = 0;

static struct
{
  uint32_t width;
  uint32_t height;
  uint16_t* data;
} font = {
  .width = FONT_WIDTH,
  .height = FONT_HEIGHT,
  .data = (uint16_t*)DEFAULT_FONT_DATA
};

static void
putch(uint32_t x, uint32_t y, char c, uint32_t bg, uint32_t fg)
{
  c -= 32;
  for (uint32_t cx = 0; cx < font.width; ++cx) 
  {
    for (uint32_t cy = 0; cy < font.height; ++cy) 
    {
      uint16_t col = (font.data[(uint64_t)c * font.width + cx] >> cy) & 1;
      framebuffer_putpix(x + cx, y + cy, col ? fg : bg);
    }
  }
}


static void
move_cursor(uint32_t x, uint32_t y)
{
  _cx = x;
  _cy = y;
}

static void
newline(void)
{
  if (_cy >= framebuffer_get_height() - 5) 
  {
    framebuffer_clear(DEFAULT_BG);
    move_cursor(0, 0);
    return;
  }
  
  move_cursor(0, _cy + (FONT_HEIGHT+4));
}

static void
putstr(const char* str, uint64_t color) 
{
  for (size_t i = 0; i < strlen(str); ++i) 
  {
    if (str[i] == '\n') 
    {
      newline();
      continue;
    }

    putch(_cx, _cy, str[i], EXTRACT_BG(color), EXTRACT_FG(color));
    move_cursor(_cx + FONT_WIDTH, _cy);
  }
}

void 
printk(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  
  uint64_t color = MAKE_FG_BG(DEFAULT_FG, DEFAULT_BG);
  for (const char* ptr = fmt; *ptr; ++ptr)
  {
    if (*ptr == '%')
    {
      ++ptr;
      switch (*ptr)
      {
        case 's':
          putstr(va_arg(ap, char*), color);
          break;
        case 'd':
        case 'u':
        case 'l':
          putstr(dec2str(va_arg(ap, uint64_t)), color);
          break;
        case 'x':
        case 'p':
          putstr(hex2str(va_arg(ap, uint64_t)), color);
          break;
        case 'X':
          putstr(hex2str(va_arg(ap, uint64_t)) + 2, color);
          break;
        case 'c':
          putstr((char[2]){va_arg(ap, int), 0}, color);
          break;
      }
    }
    else if (*ptr == '\033')
    {
      ++ptr;
      switch (*ptr)
      {
        case '0':
          color = MAKE_FG_BG(DEFAULT_FG, DEFAULT_BG);
          break;
        case '1':
          color = MAKE_FG_BG(0xB2BEB5, DEFAULT_BG);
          break;
        case '2':
          color = MAKE_FG_BG(0xFF0000, DEFAULT_BG);
          break;
        case '3':
          color = MAKE_FG_BG(0x00CED1, DEFAULT_BG);
          break;
        case '4':
          color = MAKE_FG_BG(0x9370DB, DEFAULT_BG);
          break;
      }
    }
    else
    {
      putstr((char[2]){*ptr, 0}, color);
    }
  }
  va_end(ap);
}
