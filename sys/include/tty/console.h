#ifndef _TTY_CONSOLE_H_
#define _TTY_CONSOLE_H_

#include <stdarg.h>

#define KERN_INFO "\033[1;37m[ \033[1;36mINFO \033[1;37m] "

/* TODO: Allow formatting strings */
void printk(const char* str);


#endif
