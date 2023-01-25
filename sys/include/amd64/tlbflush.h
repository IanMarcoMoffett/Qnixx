#ifndef _AMD64_TLBFLUSH_H_
#define _AMD64_TLBFLUSH_H_


#include <types.h>


void __amd64_flush_tlb_single(uintptr_t addr);
void __amd64_flush_tlb(void);


#endif
