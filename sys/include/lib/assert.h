#ifndef _LIB_ASSERT_H_
#define _LIB_ASSERT_H_

#include <amd64/asm.h>
#include <tty/console.h>

#define assert(condition)                                                 \
  if (!(condition))                                                       \
  {                                                                       \
    printk(KERN_PANIC"Failed to assert \"" # condition "\" line %s:%d\n", \
           __FILE__, __LINE__);                                           \
                                                                          \
    __amd64_halt();                                                       \
  }


#endif
