#ifndef _LIB_MATH_H_
#define _LIB_MATH_H_

#define ALIGN_DOWN(address, align)  ((address) & ~((align)-1))
#define ALIGN_UP(address, align)    (((address) + (align)-1) & ~((align)-1))
#define DIVIDE(a, b) ((a * b) == 0 ? 0 : a / b)
#define BIG_ENDIAN(v) ((((v) & 0xFF) << 8) | (((v) >> 8) & 0xFF))
#define COMBINE_WORD(word_low, word_high) (\
    (((uint64_t)word_high << 16) | word_low))

#define IS_POWER_OF_2(x) ((x & (x-1)) == 0)
#define OFF_BY(off, by) (~(off % ~(by)))
#define DIV_ROUNDUP(VALUE, DIV) ({ \
    typeof(VALUE) DIV_ROUNDUP_value = VALUE; \
    typeof(DIV) DIV_ROUNDUP_div = DIV; \
    (DIV_ROUNDUP_value + (DIV_ROUNDUP_div - 1)) / DIV_ROUNDUP_div; \
})

#endif
