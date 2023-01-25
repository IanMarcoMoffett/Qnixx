#ifndef _VIDEO_FB_H_
#define _VIDEO_FB_H_

#include <types.h>

void fb_init(void);
void framebuffer_putpix(uint32_t x, uint32_t y, uint32_t color);
uint32_t framebuffer_get_index(uint32_t x, uint32_t y);
uint32_t framebuffer_get_height(void);
uint32_t framebuffer_get_width(void);
uint32_t framebuffer_get_pitch(void);
void framebuffer_clear(uint32_t color);
uint32_t* framebuffer_get_ptr(void);

#endif
