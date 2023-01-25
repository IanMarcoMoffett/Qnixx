#ifndef _TTY_CONSOLE_H_
#define _TTY_CONSOLE_H_

#include <stdarg.h>

#define KERN_INFO "\0331[ \0333INFO\0331 ]\0330 "
#define KERN_PANIC "\0331[ \0332PANIC\0331 ]\0330 "
#define KERN_DEBUG "\0331[ \0334DEBUG\0331 ]\0330 "
#define DEFAULT_BG 0x000000
#define DEFAULT_FG 0x899499

/* TODO: Allow formatting strings */
void printk(const char* str, ...);

#endif
