#ifndef _TTY_CONSOLE_H_
#define _TTY_CONSOLE_H_

#include <stdarg.h>

#define KERN_INFO "\033[1;37m[ \033[1;36mINFO \033[1;37m] "
#define KERN_PANIC "\033[1;37m[ \033[1;31mPANIC \033[1;37m] "
#define KERN_DEBUG "\033[1;37m[ \033[1;35mDEBUG \033[1;37m] "

/* TODO: Allow formatting strings */
void printk(const char* str, ...);

#endif
