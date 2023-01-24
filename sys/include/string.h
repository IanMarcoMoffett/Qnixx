#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void memcpy(void* dst, const void* src, size_t n);
size_t strlen(const char* str);
void memzero(void* ptr, size_t n);
void memset(void* ptr, uint8_t byte, size_t n);
void memcpy(void* dst, const void* src, size_t n);
char* dec2str(size_t number);
char* hex2str(uint64_t hex_num);
size_t strlen(const char* str);
uint8_t strcmp(const char* str1, const char* str2);
uint8_t memcmp(const char* str1, const char* str2, size_t n);
void snprintf(char* str, size_t size, const char* fmt, ...);

#endif
