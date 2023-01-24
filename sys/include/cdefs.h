#ifndef _INCLUDE_CDEFS_H_
#define _INCLUDE_CDEFS_H_


#define __dead        __attribute__((noreturn))
#define __aligned(n)  __attribute__((aligned(n)))
#define __unused      __attribute__((unused))
#define __packed      __attribute__((packed))
#define __isr         __attribute__((interrupt))
#define __asm         __asm__ __volatile__

#endif
