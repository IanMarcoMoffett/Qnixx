#ifndef _AMD64_CPUID_H_
#define _AMD64_CPUID_H_


#define __cpuid(leaf, a, b, c, d)					      \
  __asm__ __volatile__ ("cpuid\n\t"					    \
			: "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
			: "0" (leaf))

#endif
