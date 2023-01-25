#include <dev/video/fb.h>
#include <limine.h>

static volatile struct limine_framebuffer_request fb_req = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

static struct limine_framebuffer* framebuffer = NULL;

uint32_t
framebuffer_get_index(uint32_t x, uint32_t y) 
{
  return x + y * (framebuffer->pitch/4);
}

uint32_t
framebuffer_get_height(void)
{
  return framebuffer->height;
}

uint32_t
framebuffer_get_width(void)
{
  return framebuffer->width;
}

uint32_t
framebuffer_get_pitch(void)
{
  return framebuffer->pitch;
}

uint32_t*
framebuffer_get_ptr(void)
{
  return framebuffer->address;
}

void
framebuffer_putpix(uint32_t x, uint32_t y, uint32_t color)
{
  uint32_t* ptr = framebuffer->address;
  ptr[framebuffer_get_index(x, y)] = color;
}

void framebuffer_clear(uint32_t color)
{
  for (size_t i = 0; i < framebuffer->width*framebuffer->height; ++i)
  {
    ((uint32_t*)framebuffer->address)[i] = color;
  }
}

void
fb_init(void)
{
  framebuffer = fb_req.response->framebuffers[0];
}
